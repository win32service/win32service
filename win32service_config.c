/*
  +----------------------------------------------------------------------+
  | PHP Version 8                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2026 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@php.net>                                    |
  | Maintainer: Jean-Baptiste Nahan <jbnahan@php.net>                    |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_win32service.h"
#include "win32service_config.h"
#include "win32service_wide.h"

char *win32_generate_path_and_params(char *path, char *params, long svc_type, char *user) {
    char *result = NULL;
	if (!path) {
        return NULL;
    }
	/* Build service path and parameters. */
    if (strchr(path, ' '))
        spprintf(&result, 0, "\"%s\" %s", path, params);
    else
        spprintf(&result, 0, "%s %s", path, params);


    /* If interact with desktop is set and no username supplied (Only LocalSystem allows InteractWithDesktop) then pass the path and params through %COMSPEC% /C "..." */
    if (result && SERVICE_INTERACTIVE_PROCESS & svc_type && user == NULL) {
        char *tmp = result;
        char *comspec = win32_internal_getenv_utf8(L"COMSPEC");
        result = NULL;
        spprintf(&result, 0, "\"%s\" /C \"%s\"", comspec ? comspec : "cmd.exe", tmp);
        if (comspec) {
            efree(comspec);
        }
        efree(tmp);
    }
	return result;
}

DWORD win32_configure_service_ex(SC_HANDLE hsvc, zval *details, BOOL is_update, DWORD start_type, char **error_msg) {
    zval *tmp;

    /* Description */
    BOOL description_changed = FALSE;
    SERVICE_DESCRIPTIONW sd;
    char *description = NULL;
    WIN32_GET_STR_DETAIL(details, INFO_DESCRIPTION, description, NULL, description_changed);
    if (description_changed || !is_update) {
        BOOL description_ok = win32_internal_to_wide(description, &sd.lpDescription);
        if (description_ok) {
            description_ok = ChangeServiceConfig2W(hsvc, SERVICE_CONFIG_DESCRIPTION, &sd);
        }
        DWORD description_error = GetLastError();
        if (sd.lpDescription) {
            efree(sd.lpDescription);
        }
        if (!description_ok) {
            *error_msg = "error when defining the description";
            return description_error;
        }
    }

    /* Delayed start */
    BOOL delayed_start_changed = FALSE;
    SERVICE_DELAYED_AUTO_START_INFO sdasi;
    WIN32_GET_BOOL_DETAIL(details, INFO_DELAYED_START, sdasi.fDelayedAutostart, FALSE, delayed_start_changed);
    if (delayed_start_changed || (!is_update && (start_type & SERVICE_AUTO_START))) {
        if (!ChangeServiceConfig2W(hsvc, SERVICE_CONFIG_DELAYED_AUTO_START_INFO, &sdasi)) {
            /* If it's an update, we might ignore the error if the service is not auto-start,
               but for now let's be strict if it was explicitly requested or during creation. */
            if (!is_update || delayed_start_changed) {
                *error_msg = "error on change the start type";
                return GetLastError();
            }
        }
    }

    /* Recovery Actions Flag */
    BOOL recovery_enabled_changed = FALSE;
    SERVICE_FAILURE_ACTIONS_FLAG sfaf;
    WIN32_GET_BOOL_DETAIL(details, INFO_RECOVERY_ENABLED, sfaf.fFailureActionsOnNonCrashFailures, FALSE, recovery_enabled_changed);
    if (recovery_enabled_changed) {
        if (!ChangeServiceConfig2W(hsvc, SERVICE_CONFIG_FAILURE_ACTIONS_FLAG, &sfaf)) {
            *error_msg = "error on change the failure action flag";
            return GetLastError();
        }
    }

    /* Recovery Actions */
    BOOL update_failure_actions = FALSE;
    SERVICE_FAILURE_ACTIONSW sfa;
    char *reboot_msg = NULL;
    char *command = NULL;
    memset(&sfa, 0, sizeof(sfa));
    SC_ACTION actions[3];
    //memset(actions, 0, sizeof(actions));

    WIN32_GET_LONG_DETAIL(details, INFO_RECOVERY_RESET_PERIOD, sfa.dwResetPeriod, (is_update ? 0 : 86400), update_failure_actions);
    WIN32_GET_STR_DETAIL(details, INFO_RECOVERY_REBOOT_MSG, reboot_msg, NULL, update_failure_actions);
    WIN32_GET_STR_DETAIL(details, INFO_RECOVERY_COMMAND, command, NULL, update_failure_actions);

    long recovery_delay;
    WIN32_GET_LONG_DETAIL(details, INFO_RECOVERY_DELAY, recovery_delay, 60000, update_failure_actions);

    long recovery_action1 = SC_ACTION_NONE;
    long recovery_action2 = SC_ACTION_NONE;
    long recovery_action3 = SC_ACTION_NONE;

    WIN32_GET_LONG_DETAIL(details, INFO_RECOVERY_ACTION_1, recovery_action1, SC_ACTION_NONE, update_failure_actions);
    WIN32_GET_LONG_DETAIL(details, INFO_RECOVERY_ACTION_2, recovery_action2, SC_ACTION_NONE, update_failure_actions);
    WIN32_GET_LONG_DETAIL(details, INFO_RECOVERY_ACTION_3, recovery_action3, SC_ACTION_NONE, update_failure_actions);

    actions[0].Type = (SC_ACTION_TYPE)recovery_action1;
    actions[0].Delay = recovery_delay;

    actions[1].Type = (SC_ACTION_TYPE)recovery_action2;
    actions[1].Delay = recovery_delay;

    actions[2].Type = (SC_ACTION_TYPE)recovery_action3;
    actions[2].Delay = recovery_delay;

    sfa.lpsaActions = actions;
    sfa.cActions = 3;

    if (update_failure_actions) {
        BOOL actions_ok = win32_internal_to_wide(reboot_msg, &sfa.lpRebootMsg) &&
                          win32_internal_to_wide(command, &sfa.lpCommand);
        if (actions_ok) {
            actions_ok = ChangeServiceConfig2W(hsvc, SERVICE_CONFIG_FAILURE_ACTIONS, &sfa);
        }
        DWORD actions_error = GetLastError();
        if (sfa.lpRebootMsg) {
            efree(sfa.lpRebootMsg);
        }
        if (sfa.lpCommand) {
            efree(sfa.lpCommand);
        }
        if (!actions_ok) {
            *error_msg = "error on change the failure action";
            return actions_error;
        }
    }

    return ERROR_SUCCESS;
}
