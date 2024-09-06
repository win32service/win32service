/*
 +----------------------------------------------------------------------+
 | PHP Version 8                                                        |
 +----------------------------------------------------------------------+
 | Copyright (c) 1997-2024 The PHP Group                                |
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

/* $Id: win32service.c 313721 2011-07-26 11:46:19Z rquadling $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "zend_exceptions.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "php_win32service.h"
#include "php_win32service_int.h"
#include "php_win32service_arginfo.h"
#include "SAPI.h"
#include "winbase.h"
#include <aclapi.h>
#include <sddl.h>
#include "win32service_right.h"
#include "win32service_right_info.h"
#include "win32service_registry.h"

#define SERVICES_REG_BASE_PRIORITY "BasePriority"


/* gargh! service_main run from a new thread that we don't spawn, so we can't do this nicely */
static void *tmp_service_g = NULL;


static DWORD WINAPI service_handler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext) {
    zend_win32service_globals *g = (zend_win32service_globals *) lpContext;
    DWORD code = NO_ERROR;

    g->args.dwControl = dwControl;
    g->args.dwEventType = dwEventType;
    g->args.lpEventData = lpEventData; /* not safe to touch without copying for later reference */

    if (dwControl == SERVICE_CONTROL_STOP) {
        g->st.dwCurrentState = SERVICE_STOP_PENDING;
    }

    SetServiceStatus(g->sh, &g->st);

    return code;
}

static void WINAPI service_main(DWORD argc, char **argv) {
    zend_win32service_globals *g = (zend_win32service_globals *) tmp_service_g;
    DWORD base_priority;
    HKEY hKey;
    char *service_key;
    long registry_result = ERROR_SUCCESS;
    DWORD dwType = REG_DWORD;
    DWORD dwSize = sizeof(DWORD);

    // Set the base priority for this service.
    /*spprintf(&service_key, 0, "%s%s", SERVICES_REG_KEY_ROOT, g->service_name);

    registry_result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, service_key, 0, KEY_ALL_ACCESS, &hKey);
    if (ERROR_SUCCESS == registry_result) {
        registry_result = RegQueryValueEx(hKey, SERVICES_REG_BASE_PRIORITY, 0, &dwType, (LPBYTE)&base_priority, &dwSize);
    }

    efree(service_key);

    if (hKey) {
        RegCloseKey(hKey);
    }

    if (ERROR_SUCCESS != registry_result) {
        g->code = registry_result;
        SetEvent(g->event);
        return;
    }

    if(!SetPriorityClass(GetCurrentProcess(), base_priority)) {
        g->code = GetLastError();
        SetEvent(g->event);
        return;
    }*/

    g->st.dwServiceType = SERVICE_WIN32;
    g->st.dwCurrentState = SERVICE_START_PENDING;
    g->st.dwControlsAccepted = g->dwControlsAccepted;

    g->sh = RegisterServiceCtrlHandlerEx(g->service_name, service_handler, g);

    if (g->sh == (SERVICE_STATUS_HANDLE)0) {
        g->code = GetLastError();
        SetEvent(g->event);
        return;
    }

    g->code = NO_ERROR;
    SetEvent(g->event);
}

static DWORD WINAPI svc_thread_proc(LPVOID _globals) {
    zend_win32service_globals *g = (zend_win32service_globals *) _globals;

    tmp_service_g = g;

    if (!StartServiceCtrlDispatcher(g->te)) {
        g->code = GetLastError();
        SetEvent(g->event);
        return 1;
    }

    /* not reached until service_main returns */
    return 0;
}

static void convert_error_to_exception(DWORD code, const char *message) {
    if (code == ERROR_ACCESS_DENIED) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error access denied (%s)", message);
        return;
    }
    if (code == ERROR_CIRCULAR_DEPENDENCY) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error circular dependency (%s)", message);
        return;
    }
    if (code == ERROR_DATABASE_DOES_NOT_EXIST) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error database does not exist (%s)", message);
        return;
    }
    if (code == ERROR_DEPENDENT_SERVICES_RUNNING) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error dependent services running (%s)", message);
        return;
    }
    if (code == ERROR_DUPLICATE_SERVICE_NAME) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error duplicate service name (%s)", message);
        return;
    }
    if (code == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error failed service controller connect (%s)",
                                message);
        return;
    }
    if (code == ERROR_INSUFFICIENT_BUFFER) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error insufficient buffer (%s)", message);
        return;
    }
    if (code == ERROR_INVALID_DATA) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error invalid data (%s)", message);
        return;
    }
    if (code == ERROR_INVALID_HANDLE) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error invalid handle (%s)", message);
        return;
    }
    if (code == ERROR_INVALID_LEVEL) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error invalid level (%s)", message);
        return;
    }
    if (code == ERROR_INVALID_NAME) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error invalid name (%s)", message);
        return;
    }
    if (code == ERROR_INVALID_PARAMETER) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error invalid parameter (%s)", message);
        return;
    }
    if (code == ERROR_INVALID_SERVICE_ACCOUNT) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error invalid service account (%s)", message);
        return;
    }
    if (code == ERROR_INVALID_SERVICE_CONTROL) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error invalid service control (%s)", message);
        return;
    }
    if (code == ERROR_PATH_NOT_FOUND) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error path not found (%s)", message);
        return;
    }
    if (code == ERROR_SERVICE_ALREADY_RUNNING) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error service already running (%s)", message);
        return;
    }
    if (code == ERROR_SERVICE_CANNOT_ACCEPT_CTRL) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error service cannot accept ctrl (%s)", message);
        return;
    }
    if (code == ERROR_SERVICE_DATABASE_LOCKED) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error service database locked (%s)", message);
        return;
    }
    if (code == ERROR_SERVICE_DEPENDENCY_DELETED) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error service dependency deleted (%s)", message);
        return;
    }
    if (code == ERROR_SERVICE_DEPENDENCY_FAIL) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error service dependency fail (%s)", message);
        return;
    }
    if (code == ERROR_SERVICE_DISABLED) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error service disabled (%s)", message);
        return;
    }
    if (code == ERROR_SERVICE_DOES_NOT_EXIST) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error service does not exist (%s)", message);
        return;
    }
    if (code == ERROR_SERVICE_EXISTS) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error service exists (%s)", message);
        return;
    }
    if (code == ERROR_SERVICE_LOGON_FAILED) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error service logon failed (%s)", message);
        return;
    }
    if (code == ERROR_SERVICE_MARKED_FOR_DELETE) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error service marked for delete (%s)", message);
        return;
    }
    if (code == ERROR_SERVICE_NO_THREAD) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error service no thread (%s)", message);
        return;
    }
    if (code == ERROR_SERVICE_NOT_ACTIVE) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error service not active (%s)", message);
        return;
    }
    if (code == ERROR_SERVICE_REQUEST_TIMEOUT) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error service request timeout (%s)", message);
        return;
    }
    if (code == ERROR_SHUTDOWN_IN_PROGRESS) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error shutdown in progress (%s)", message);
        return;
    }
    if (code == ERROR_SERVICE_SPECIFIC_ERROR) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Error service specific error (%s)", message);
        return;
    }
    if (code == ERROR_NONE_MAPPED) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code,
                                "No mapping between account names and security IDs was done. (%s)", message);
        return;
    }
    if (code == 16000) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Internal extension error (%s)", message);
        return;
    }
    if (code != NO_ERROR) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, code, "Unknow error no %d (%s)", code, message);
        return;
    }
}

static bool win32_check_if_is_service() {
    HWINSTA hWinStation = GetProcessWindowStation();
    if (hWinStation != NULL) {
        USEROBJECTFLAGS uof;
        DWORD lenNeeded;
        if (GetUserObjectInformation(hWinStation, UOI_FLAGS, &uof, sizeof(uof), &lenNeeded)) {
            return ((uof.dwFlags & WSF_VISIBLE) == 0 && GetConsoleWindow() == NULL);
        }
    }
    return FALSE;
}

/* {{{ proto bool win32_start_service_ctrl_dispatcher(string $name, [bool gracefulExit])
   Registers the script with the SCM, so that it can act as the service with the given name */
static PHP_FUNCTION(win32_start_service_ctrl_dispatcher) {
    if (win32_check_if_is_service() == FALSE && strcmp(sapi_module.name, "embed") != 0) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, 0,
                                "This function work only when using the CLI SAPI and called into the service code.");
        RETURN_THROWS();
    }

    char *name;
    size_t name_len;
    zend_bool gracefulExitParam = SVCG(gracefulExit);

    if (SVCG(svc_thread)) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, 0, "Service ctrl dispatcher already running");
        RETURN_THROWS();
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s|b", &name, &name_len, &gracefulExitParam)) {
        RETURN_THROWS();
    }

    if (name_len == 0) {
        zend_argument_value_error(1, "the value cannot be empty");
        RETURN_THROWS();
    }

    SVCG(service_name) = estrdup(name);

    SVCG(gracefulExit) = gracefulExitParam;

    SVCG(te)[0].lpServiceName = SVCG(service_name);
    SVCG(te)[0].lpServiceProc = service_main;
    SVCG(event) = CreateEvent(NULL, TRUE, FALSE, NULL);

    SVCG(svc_thread) = CreateThread(NULL, 0, svc_thread_proc, &SVCG(svc_thread), 0, &SVCG(svc_thread_id));

    if (SVCG(svc_thread) == NULL) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, 0, "Failed to start dispatcher thread");
        RETURN_THROWS();
    }

    if (WAIT_OBJECT_0 == WaitForSingleObject(SVCG(event), 30000)) {
        if (SVCG(code) == NO_ERROR) {
            RETURN_TRUE;
        }

        CloseHandle(SVCG(svc_thread));
        SVCG(svc_thread) = NULL;
        convert_error_to_exception(SVCG(code), "");
        RETURN_THROWS();
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool win32_set_service_pause_resume_state([bool enable])
    */
static PHP_FUNCTION(win32_set_service_pause_resume_state) {
    if (win32_check_if_is_service() == FALSE && strcmp(sapi_module.name, "embed") != 0) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, 0,
                                "This function work only when using the CLI SAPI and called into the service code.");
        RETURN_THROWS();
    }
    DWORD dwControlsAccepted = SVCG(dwControlsAccepted);
    zend_bool enable;
    zend_bool previous_state = (dwControlsAccepted & SERVICE_ACCEPT_PAUSE_CONTINUE) != 0;;

    if (SVCG(svc_thread) && ZEND_NUM_ARGS() > 0) {
        zend_argument_value_error(1, "Unable to change the pause/resume state when control dispatcher is already running. Call without argument if you want the state or call before win32_start_service_ctrl_dispatcher function.");
        RETURN_THROWS();
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &enable)) {
        RETURN_THROWS();
    }

    if (ZEND_NUM_ARGS() > 0) {
        if (enable) {
            // Set the SERVICE_ACCEPT_PAUSE_CONTINUE bit
            dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
        } else {
            // Clear the SERVICE_ACCEPT_PAUSE_CONTINUE bit
            dwControlsAccepted &= ~SERVICE_ACCEPT_PAUSE_CONTINUE;
        }
        SVCG(dwControlsAccepted) = dwControlsAccepted;
    }

    RETURN_BOOL(previous_state);
}
/* }}} */

/* {{{ proto bool win32_set_service_exit_mode([bool gracefulExit])
   Set (and get) the exit mode of the service, when set to true the service
   will shut down gracefuly when PHP exits, when set to false it will not shut
   down gracefuly, this will mean that the service will count as having failed
   and the recovery action will be run */
static PHP_FUNCTION(win32_set_service_exit_mode) {
    if (win32_check_if_is_service() == FALSE && strcmp(sapi_module.name, "embed") != 0) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, 0,
                                "This function work only when using the CLI SAPI and called into the service code.");
        RETURN_THROWS();
    }
    zend_bool gracefulExitParam = SVCG(gracefulExit);
    zend_bool old_gracefulExitParam = SVCG(gracefulExit);

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &gracefulExitParam)) {
        RETURN_THROWS();
    }

    SVCG(gracefulExit) = gracefulExitParam;

    RETURN_BOOL(old_gracefulExitParam);
}
/* }}} */

/* {{{ proto long win32_set_service_exit_code([int exitCode])
   Set (and get) the exit code of the service, when the service will shut down
   gracefuly when PHP exits it's not used, when the service will shut down not
   gracefuly the int is used for exitCode and the recovery action will be run
   if exit code is not zero */
static PHP_FUNCTION(win32_set_service_exit_code) {
    if (win32_check_if_is_service() == FALSE && strcmp(sapi_module.name, "embed") != 0) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, 0,
                                "This function work only when using the CLI SAPI and called into the service code.");
        RETURN_THROWS();
    }
    zend_long exitCodeParam = SVCG(gracefulExit);
    zend_long old_exitCodeParam = SVCG(gracefulExit);

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &exitCodeParam)) {
        RETURN_THROWS();
    }

    SVCG(exitCode) = exitCodeParam;

    RETURN_LONG(old_exitCodeParam);
}
/* }}} */

/* {{{ proto bool win32_set_service_status(int status, [int checkpoint])
   Update the service status */
static PHP_FUNCTION(win32_set_service_status) {
    if (win32_check_if_is_service() == FALSE && strcmp(sapi_module.name, "embed") != 0) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, 0,
                                "This function work only when using the CLI SAPI and called into the service code.");
        RETURN_THROWS();
    }

    long status;
    long checkpoint = 0;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &status, &checkpoint)) {
        RETURN_THROWS();
    }

    SVCG(st.dwCurrentState) = status;
    /* CheckPoints are only valid for the SERVICE_*_PENDING statuses. */
    if ((status == SERVICE_CONTINUE_PENDING) || (status == SERVICE_PAUSE_PENDING) ||
        (status == SERVICE_START_PENDING) || (status == SERVICE_STOP_PENDING)) {
        SVCG(st.dwCheckPoint) = checkpoint;
    }

    if (!SetServiceStatus(SVCG(sh), &SVCG(st))) {
        convert_error_to_exception(GetLastError(), "");
        RETURN_THROWS();
    } else {
        RETURN_TRUE;
    }
}
/* }}} */

/* {{{ proto object win32_read_right_access_service(string servicename, string username [, string machine])
   Read right to service for account */
static PHP_FUNCTION(win32_read_right_access_service) {
    char *machine = NULL;
    size_t machine_len;
    char *service = NULL;
    size_t service_len;
    char *username = NULL;
    size_t username_len;
    PSECURITY_DESCRIPTOR pSD = NULL;
    ACCESS_MASK pAccess;
    PACL pACL = NULL;
    TRUSTEE trustee;
    DWORD dwRes;

    if (FAILURE ==
        zend_parse_parameters(ZEND_NUM_ARGS(), "ss|s!", &service, &service_len, &username, &username_len, &machine,
                              &machine_len)) {
        RETURN_THROWS();
    }

    dwRes = GetNamedSecurityInfo(service, SE_SERVICE,
                                 DACL_SECURITY_INFORMATION,
                                 NULL, NULL, &pACL, NULL, &pSD);

    if (ERROR_SUCCESS != dwRes) {
        if (pSD != NULL) { LocalFree((HLOCAL) pSD); }

        convert_error_to_exception(dwRes, "GetNamedSecurityInfo Error");
        RETURN_THROWS();
    }

    if (pACL == NULL) {
        if (pSD != NULL) { LocalFree((HLOCAL) pSD); }
        convert_error_to_exception(16000, "ACL empty");
        RETURN_THROWS();
    }


    ZeroMemory(&trustee, sizeof(PTRUSTEE_A));

    BuildTrusteeWithName(&trustee, username);

    dwRes = GetEffectiveRightsFromAcl(pACL, &trustee, &pAccess);
    if (ERROR_SUCCESS != dwRes) {
        if (pSD != NULL) { LocalFree((HLOCAL) pSD); }
        convert_error_to_exception(dwRes, "GetEffectiveRightsFromAcl Error");
        RETURN_THROWS();
    }
    zval result;
    array_init(&result);
    php_explode(zend_string_init(ZEND_STRL("\\"), 0), zend_string_init(username, username_len, 0), &result, 2);

    char *explDomain = NULL;
    long explDomain_len = 0;
    char *explUsername = NULL;
    long explUsername_len = 0;

    HashPosition pos;
    zend_hash_internal_pointer_reset_ex(Z_ARRVAL(result), &pos);

    if (zend_hash_num_elements(Z_ARRVAL(result)) == 1) {
        zval * element = zend_hash_get_current_data_ex(Z_ARRVAL(result), &pos);
        explUsername = Z_STRVAL_P(element);
        explUsername_len = Z_STRLEN_P(element);
    }
    if (zend_hash_num_elements(Z_ARRVAL(result)) == 2) {
        zval * element = zend_hash_get_current_data_ex(Z_ARRVAL(result), &pos);
        explDomain = Z_STRVAL_P(element);
        explDomain_len = Z_STRLEN_P(element);

        zval * element2 = zend_hash_get_current_data_ex(Z_ARRVAL(result), &pos);
        explUsername = Z_STRVAL_P(element2);
        explUsername_len = Z_STRLEN_P(element2);
    }
    win32service_create_right_info(return_value, explDomain, explDomain_len, explUsername, explUsername_len, pAccess,
                                   GRANT_ACCESS);
    if (pSD != NULL)
        LocalFree((HLOCAL) pSD);
    efree(explDomain);
    efree(explUsername);
}
/* }}} */


/* {{{ proto object win32_read_all_rights_access_service(string servicename [, string machine])
   Read all rights to service */
static PHP_FUNCTION(win32_read_all_rights_access_service) {
    char *machine = NULL;
    size_t machine_len;
    char *service = NULL;
    size_t service_len;
    PSECURITY_DESCRIPTOR pSD = NULL;
    ACCESS_MASK pAccess;
    PACL pACL = NULL;
    TRUSTEE trustee;
    DWORD dwRes;
    long pcCountOfExplicitEntries = 0;
    PEXPLICIT_ACCESS_A pListOfExplicitEntries = NULL;
    long index = 0;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s|s!", &service, &service_len, &machine, &machine_len)) {
        RETURN_THROWS();
    }

    dwRes = GetNamedSecurityInfo(service, SE_SERVICE,
                                 DACL_SECURITY_INFORMATION,
                                 NULL, NULL, &pACL, NULL, &pSD);

    if (ERROR_SUCCESS != dwRes) {
        if (pSD != NULL) { LocalFree((HLOCAL) pSD); }

        convert_error_to_exception(dwRes, "GetNamedSecurityInfo Error");
        RETURN_THROWS();
    }

    if (pACL == NULL) {
        if (pSD != NULL) { LocalFree((HLOCAL) pSD); }
        convert_error_to_exception(16000, "ACL empty");
        RETURN_THROWS();
    }

    dwRes = GetExplicitEntriesFromAcl(
            pACL,
            &pcCountOfExplicitEntries,
            &pListOfExplicitEntries
    );
//    printf("GetExplicitEntriesFromAcl OK, count: %ld\n",pcCountOfExplicitEntries);
    if (ERROR_SUCCESS != dwRes) {
        if (pSD != NULL) { LocalFree((HLOCAL) pSD); }

        convert_error_to_exception(dwRes, "GetExplicitEntriesFromAcl Error");
        RETURN_THROWS();
    }
    char *sid;
    char name[256];
    long dwSize;
    char domainName[256];
    long dwDomaineSize;

    SID_NAME_USE peUse;

    array_init_size(return_value, (uint32_t)pcCountOfExplicitEntries);

    for (index = 0; index < pcCountOfExplicitEntries; index++) {
        zval result;
        ZVAL_UNDEF(&result);
        dwSize = 256;
        dwDomaineSize = 256;
        if ((pListOfExplicitEntries[index].Trustee.TrusteeForm & TRUSTEE_IS_SID) != TRUSTEE_IS_SID) {
            continue;
        }
        if (!ConvertSidToStringSidA(pListOfExplicitEntries[index].Trustee.ptstrName, &sid)) {
            continue;
        }
        if (!IsValidSid(pListOfExplicitEntries[index].Trustee.ptstrName)) {
            continue;
        }

        if (LookupAccountSid(
                machine,
                pListOfExplicitEntries[index].Trustee.ptstrName,
                name,
                &dwSize,
                domainName,
                &dwDomaineSize,
                &peUse
        )) {
//            printf("name: %s domain: %s ", name, domainName);
            char *domainNamePtr = &domainName[0];
            char *namePtr = &name[0];
            win32service_create_right_info(&result, domainNamePtr, dwDomaineSize, namePtr, dwSize,
                                           pListOfExplicitEntries[index].grfAccessPermissions,
                                           pListOfExplicitEntries[index].grfAccessMode);
//            printf(" init OK");
            add_next_index_zval(return_value, &result);
//            printf(" add in array OK\n");
        } else {
//            printf("SID: %s %d nameSize: %d domain size: %d\n", sid, GetLastError(), dwSize, dwDomaineSize);
            win32service_create_right_info(&result, NULL, 0, sid, strlen(sid),
                                           pListOfExplicitEntries[index].grfAccessPermissions,
                                           pListOfExplicitEntries[index].grfAccessMode);
            add_next_index_zval(return_value, &result);
        }
        LocalFree((HLOCAL) sid);
    }

    if (pSD != NULL)
        LocalFree((HLOCAL) pSD);

}
/* }}} */

/* {{{ proto void win32_add_right_access_service(string servicename, string username, int right [, string machine])
   Change right to service for account */
static PHP_FUNCTION(win32_add_right_access_service) {
    char *machine = NULL;
    size_t machine_len;
    char *service = NULL;
    size_t service_len;
    char *username = NULL;
    size_t username_len;
    long right = 0;
    long result = 0;
    char *errorMessage = NULL;

    if (FAILURE ==
        zend_parse_parameters(ZEND_NUM_ARGS(), "ssl|s!", &service, &service_len, &username, &username_len, &right,
                              &machine, &machine_len)) {
        RETURN_THROWS();
    }

    if (service == NULL || strlen(service) < 2) {
        zend_argument_value_error(1, "the value of argument 'serviceName' cannot be empty");
        RETURN_THROWS();
    }

    if (username == NULL || strlen(username) < 2) {
        zend_argument_value_error(1, "the value of argument 'username' cannot be empty");
        RETURN_THROWS();
    }

    if (right <= 0 || right > 0xF01FF) {
        zend_argument_value_error(1, "the value of argument 'right' must between 0x00001 and 0xF01FF");
        RETURN_THROWS();
    }
    errorMessage = emalloc(sizeof(char) * 150);
    result = change_service_right(service, username, right, SET_ACCESS, machine, errorMessage);
    if (result != 0) {
        convert_error_to_exception(result, errorMessage);
        RETURN_THROWS();
    }

}
/* }}} */

/* {{{ proto void win32_remove_right_access_service(string servicename, string username [, string machine])
   Change right to service for account */
static PHP_FUNCTION(win32_remove_right_access_service) {
    char *machine = NULL;
    size_t machine_len;
    char *service = NULL;
    size_t service_len;
    char *username = NULL;
    size_t username_len;
    long result = 0;
    char *errorMessage = NULL;

    if (FAILURE ==
        zend_parse_parameters(ZEND_NUM_ARGS(), "ss|s!", &service, &service_len, &username, &username_len, &machine,
                              &machine_len)) {
        RETURN_THROWS();
    }

    if (service == NULL || strlen(service) < 2) {
        zend_argument_value_error(1, "the value of argument 'serviceName' cannot be empty");
        RETURN_THROWS();
    }

    if (username == NULL || strlen(username) < 2) {
        zend_argument_value_error(1, "the value of argument 'username' cannot be empty");
        RETURN_THROWS();
    }
    errorMessage = emalloc(sizeof(char) * 150);
    result = change_service_right(service, username, SERVICE_ALL_ACCESS, REVOKE_ACCESS, machine, errorMessage);
    if (result != 0) {
        convert_error_to_exception(result, errorMessage);
        RETURN_THROWS();
    }
}
/* }}} */

/* {{{ proto void win32_create_service(array details [, string machine])
   Creates a new service entry in the SCM database */
static PHP_FUNCTION(win32_create_service) {
    zval * tmp;
    zval * details;
    char *machine = NULL;
    size_t machine_len;
    char *service = NULL;
    char *display;
    char *user;
    char *password;
    char *path;
    char *params;
    long svc_type;
    long start_type;
    long error_control;
    long recovery_action1;
    long recovery_action2;
    long recovery_action3;
    long recovery_delay;
    char *recovery_reboot_msg = NULL;
    char *recovery_command = NULL;
    long recovery_reset_period;

    char *load_order;
    char *deps = NULL;
    char *desc;
    BOOL delayed_start;
    BOOL recovery_enabled;
    SC_HANDLE hsvc, hmgr;
    char *path_and_params;
    SERVICE_DESCRIPTION srvc_desc;
    SERVICE_DELAYED_AUTO_START_INFO srvc_delayed_start;
    SERVICE_FAILURE_ACTIONS srvc_failure_infos;
    SERVICE_FAILURE_ACTIONS_FLAG srvc_failure_action;
    DWORD base_priority;
    HKEY hKey;
    char *service_key;
    long registry_result;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "a|s!", &details, &machine, &machine_len)) {
        RETURN_THROWS();
    }

#define STR_DETAIL(name, var, def) \
    if ((tmp = zend_hash_str_find(Z_ARRVAL_P(details), name, sizeof(name)-1)) != NULL) { \
        if (IS_NULL != Z_TYPE_P(tmp)) { \
            convert_to_string_ex(tmp); \
        } else { \
            convert_to_null_ex(tmp); \
        } \
        if (strlen(Z_STRVAL_P(tmp)) != Z_STRLEN_P(tmp)) { \
            php_error_docref(NULL, E_WARNING, "malformed " name); \
            RETURN_FALSE; \
        } \
        var = Z_STRVAL_P(tmp); \
    } else { \
        var = def; \
    }

#define INT_DETAIL(name, var, def) \
    if ((tmp = zend_hash_find(Z_ARRVAL_P(details), zend_string_init(name, strlen(name), 0))) != NULL) { \
        convert_to_long_ex(tmp); \
        var = Z_LVAL_P(tmp); \
    } else { \
        var = def; \
    }

#define BOOL_DETAIL(name, var, def) \
    if ((tmp = zend_hash_find(Z_ARRVAL_P(details), zend_string_init(name, strlen(name), 0))) != NULL) { \
        convert_to_boolean_ex(tmp); \
        var = Z_TYPE_P(tmp) == IS_TRUE ? 1 : 0; \
    } else { \
        var = def; \
    }

#define ARRAY_TO_STR_DETAIL(name, var, def) \
    if ((tmp = zend_hash_find(Z_ARRVAL_P(details), zend_string_init(name, strlen(name), 0))) != NULL) { \
        if (Z_TYPE_P(tmp) == IS_STRING) { \
            STR_DETAIL(name, var, def); \
        } else if (Z_TYPE_P(tmp) == IS_ARRAY) { \
            HashTable * myht = Z_ARRVAL_P(tmp); \
            uint32_t  count = zend_array_count(myht); \
            if (count == 0) { \
                var = NULL; \
            } else { \
                zend_ulong num; \
                zend_string *key; \
                zval *val; \
                uint32_t totalLenth = 0; \
                char ** var2 = emalloc(sizeof(char*) * count); \
                ZEND_HASH_FOREACH_KEY_VAL_IND(myht, num, key, val) { \
                    convert_to_string_ex(val); \
                    totalLenth += strlen(Z_STRVAL_P(val)) + 1; \
                    var2[num] = Z_STRVAL_P(val); \
                } ZEND_HASH_FOREACH_END(); \
                totalLenth++; \
                var = emalloc(sizeof(char)*totalLenth); \
                uint32_t j = 0; \
                for (uint32_t l = 0; l < count; l++) { \
                    for (uint32_t k = 0; k <= strlen(var2[l]); k++) { \
                        var[j] = var2[l][k]; \
                        j++; \
                    } \
                    var[j] = '\0'; \
                } \
                var[totalLenth - 1] = '\0'; \
            } \
        } else { \
            var = def; \
        } \
    } else { \
        var = def; \
    }


    STR_DETAIL(INFO_SERVICE, service, NULL);
    STR_DETAIL(INFO_DISPLAY, display, NULL);
    STR_DETAIL(INFO_USER, user, NULL);
    STR_DETAIL(INFO_PASSWORD, password, "");
    STR_DETAIL(INFO_PATH, path, NULL);
    STR_DETAIL(INFO_PARAMS, params, "");
    STR_DETAIL(INFO_LOAD_ORDER, load_order, NULL);
    ARRAY_TO_STR_DETAIL(INFO_DEPENDENCIES, deps, NULL);
    STR_DETAIL(INFO_DESCRIPTION, desc, NULL);
    INT_DETAIL(INFO_SVC_TYPE, svc_type, SERVICE_WIN32_OWN_PROCESS);
    INT_DETAIL(INFO_START_TYPE, start_type, SERVICE_AUTO_START);
    INT_DETAIL(INFO_ERROR_CONTROL, error_control, SERVICE_ERROR_IGNORE);
    BOOL_DETAIL(INFO_DELAYED_START, delayed_start, 0); /* Allow Vista+ delayed service start. */
    INT_DETAIL(INFO_BASE_PRIORITY, base_priority, NORMAL_PRIORITY_CLASS);
    INT_DETAIL(INFO_RECOVERY_DELAY, recovery_delay, 60000);
    INT_DETAIL(INFO_RECOVERY_ACTION_1, recovery_action1, SC_ACTION_NONE);
    INT_DETAIL(INFO_RECOVERY_ACTION_2, recovery_action2, SC_ACTION_NONE);
    INT_DETAIL(INFO_RECOVERY_ACTION_3, recovery_action3, SC_ACTION_NONE);
    INT_DETAIL(INFO_RECOVERY_RESET_PERIOD, recovery_reset_period, 86400);
    BOOL_DETAIL(INFO_RECOVERY_ENABLED, recovery_enabled, 0);
    STR_DETAIL(INFO_RECOVERY_REBOOT_MSG, recovery_reboot_msg, NULL);
    STR_DETAIL(INFO_RECOVERY_COMMAND, recovery_command, NULL);


    if (service == NULL) {
        zend_argument_value_error(1, "the value for key '%s' is wrong", INFO_SERVICE);
        RETURN_THROWS();
    }

    if (strlen(service) < 2) {
        zend_argument_value_error(1, "the value for key '%s' cannot be empty", INFO_SERVICE);
        RETURN_THROWS();
    }

    if (path == NULL) {
        zend_argument_value_error(1, "the value for key '%s' is wrong", INFO_PATH);
        RETURN_THROWS();
    }

    if (svc_type != SERVICE_WIN32_OWN_PROCESS &&
        svc_type != SERVICE_INTERACTIVE_PROCESS &&
        svc_type != SERVICE_WIN32_OWN_PROCESS_INTERACTIVE) {
        zend_argument_value_error(1,
                                  "the value %d for '%s' key is wrong, Use WIN32_SERVICE_WIN32_OWN_PROCESS, WIN32_SERVICE_INTERACTIVE_PROCESS or WIN32_SERVICE_WIN32_OWN_PROCESS_INTERACTIVE constants",
                                  svc_type,
                                  INFO_SVC_TYPE);
        RETURN_THROWS();
    }
    if (start_type != SERVICE_BOOT_START &&
        start_type != SERVICE_SYSTEM_START &&
        start_type != SERVICE_AUTO_START &&
        start_type != SERVICE_DEMAND_START &&
        start_type != SERVICE_DISABLED) {
        zend_argument_value_error(1,
                                  "the value %d for '%s' key is wrong, Use WIN32_SERVICE_BOOT_START, WIN32_SERVICE_SYSTEM_START, WIN32_SERVICE_AUTO_START, WIN32_SERVICE_DEMAND_START or WIN32_SERVICE_DISABLED constants",
                                  start_type,
                                  INFO_START_TYPE);
        RETURN_THROWS();
    }

    if (error_control != SERVICE_ERROR_IGNORE &&
        error_control != SERVICE_ERROR_NORMAL &&
        error_control != SERVICE_ERROR_SEVERE &&
        error_control != SERVICE_ERROR_CRITICAL) {
        zend_argument_value_error(1,
                                  "the value %d for '%s' key is wrong, Use WIN32_SERVICE_ERROR_IGNORE, WIN32_SERVICE_ERROR_NORMAL, WIN32_SERVICE_ERROR_SEVERE or WIN32_SERVICE_ERROR_CRITICAL constants",
                                  error_control,
                                  INFO_ERROR_CONTROL);
        RETURN_THROWS();
    }
    if (base_priority != ABOVE_NORMAL_PRIORITY_CLASS &&
        base_priority != BELOW_NORMAL_PRIORITY_CLASS &&
        base_priority != HIGH_PRIORITY_CLASS &&
        base_priority != IDLE_PRIORITY_CLASS &&
        base_priority != NORMAL_PRIORITY_CLASS &&
        base_priority != REALTIME_PRIORITY_CLASS) {
        zend_argument_value_error(1,
                                  "the value %d for '%s' key is wrong, Use WIN32_ABOVE_NORMAL_PRIORITY_CLASS, WIN32_BELOW_NORMAL_PRIORITY_CLASS, WIN32_HIGH_PRIORITY_CLASS, WIN32_IDLE_PRIORITY_CLASS, WIN32_NORMAL_PRIORITY_CLASS or WIN32_REALTIME_PRIORITY_CLASS constants",
                                  base_priority,
                                  INFO_BASE_PRIORITY);
        RETURN_THROWS();
    }

    if (recovery_delay < 0 || recovery_delay > ZEND_LONG_MAX) {
        zend_argument_value_error(1,
                                  "the value for key '%s' must between 0 and " ZEND_LONG_FMT
        ". Got %d.",
                INFO_RECOVERY_DELAY,
                ZEND_LONG_MAX,
                recovery_delay);
        RETURN_THROWS();
    }


    if (recovery_action1 != SC_ACTION_NONE && recovery_action1 != SC_ACTION_REBOOT &&
        recovery_action1 != SC_ACTION_RESTART && recovery_action1 != SC_ACTION_RUN_COMMAND) {
        zend_argument_value_error(1,
                                  "the value %d for '%s' key is wrong. Use WIN32_SC_ACTION_NONE, WIN32_SC_ACTION_REBOOT, WIN32_SC_ACTION_RESTART or WIN32_SC_ACTION_RUN_COMMAND constants",
                                  recovery_action1,
                                  INFO_RECOVERY_ACTION_1);
        RETURN_THROWS();
    }

    if (recovery_action2 != SC_ACTION_NONE && recovery_action2 != SC_ACTION_REBOOT &&
        recovery_action2 != SC_ACTION_RESTART && recovery_action2 != SC_ACTION_RUN_COMMAND) {
        zend_argument_value_error(1,
                                  "the value %d for '%s' key is wrong. Use WIN32_SC_ACTION_NONE, WIN32_SC_ACTION_REBOOT, WIN32_SC_ACTION_RESTART or WIN32_SC_ACTION_RUN_COMMAND constants",
                                  recovery_action2,
                                  INFO_RECOVERY_ACTION_2);
        RETURN_THROWS();
    }

    if (recovery_action3 != SC_ACTION_NONE && recovery_action3 != SC_ACTION_REBOOT &&
        recovery_action3 != SC_ACTION_RESTART && recovery_action3 != SC_ACTION_RUN_COMMAND) {
        zend_argument_value_error(1,
                                  "the value %d for '%s' key is wrong. Use WIN32_SC_ACTION_NONE, WIN32_SC_ACTION_REBOOT, WIN32_SC_ACTION_RESTART or WIN32_SC_ACTION_RUN_COMMAND constants",
                                  recovery_action3,
                                  INFO_RECOVERY_ACTION_3);
        RETURN_THROWS();
    }
    if (recovery_reset_period < 0 || recovery_reset_period > ZEND_LONG_MAX) {
        zend_argument_value_error(1,
                                  "the value for key '%s' must between 0 and " ZEND_LONG_FMT
        ". Got %d.",
                INFO_RECOVERY_RESET_PERIOD,
                ZEND_LONG_MAX,
                recovery_reset_period);
        RETURN_THROWS();
    }


    srvc_desc.lpDescription = desc;
    srvc_delayed_start.fDelayedAutostart = delayed_start;
    srvc_failure_infos.lpRebootMsg = NULL;
    srvc_failure_infos.lpCommand = NULL;

    srvc_failure_infos.dwResetPeriod = recovery_reset_period;
    if (recovery_reboot_msg != NULL) {
        srvc_failure_infos.lpRebootMsg = recovery_reboot_msg;
    }
    if (recovery_command != NULL) {
        srvc_failure_infos.lpCommand = recovery_command;
    }

    srvc_failure_infos.cActions = 3;
    SC_ACTION recovery_actions[3];
    recovery_actions[0].Delay = recovery_delay;
    recovery_actions[0].Type = recovery_action1;
    recovery_actions[1].Delay = recovery_delay;
    recovery_actions[1].Type = recovery_action2;
    recovery_actions[2].Delay = recovery_delay;
    recovery_actions[2].Type = recovery_action3;

    srvc_failure_infos.lpsaActions = recovery_actions;
    srvc_failure_action.fFailureActionsOnNonCrashFailures = recovery_enabled;

    /* Connect to the SCManager. */
    hmgr = OpenSCManager(machine, NULL, SC_MANAGER_ALL_ACCESS);

    /* Quit if no connection. */
    if (!hmgr) {
        convert_error_to_exception(GetLastError(), "");
        RETURN_THROWS();
    }

    /* Build service path and parameters. */
    if (path == NULL) {
        DWORD len;
        char buf[MAX_PATH];

        len = GetModuleFileName(NULL, buf, sizeof(buf));
        buf[len] = '\0';

        if (strchr(buf, ' '))
            spprintf(&path_and_params, 0, "\"%s\" %s", buf, params);
        else
            spprintf(&path_and_params, 0, "%s %s", buf, params);
    } else {
        if (strchr(path, ' '))
            spprintf(&path_and_params, 0, "\"%s\" %s", path, params);
        else
            spprintf(&path_and_params, 0, "%s %s", path, params);
    }

    /* If interact with desktop is set and no username supplied (Only LocalSystem allows InteractWithDesktop) then pass the path and params through %COMSPEC% /C "..." */
    if (SERVICE_INTERACTIVE_PROCESS & svc_type && user == NULL) {
        spprintf(&path_and_params, 0, "\"%s\" /C \"%s\"", getenv("COMSPEC"), path_and_params);
    }

    /* Register the service. */
    hsvc = CreateService(hmgr,
                         service,
                         display ? display : service,
                         SERVICE_ALL_ACCESS,
                         svc_type,
                         start_type,
                         error_control,
                         path_and_params,
                         load_order,
                         NULL,
                         (LPCSTR) deps,
                         (LPCSTR) user,
                         (LPCSTR) password);

    efree(path_and_params);


    /* If there was an error :
       - Creating the service
       - Setting the service description
       - Setting the delayed start
       then track the error. */
    if (!hsvc) {
        CloseServiceHandle(hmgr);
        convert_error_to_exception(GetLastError(), "on create service");
        RETURN_THROWS();
    }

    if (!ChangeServiceConfig2(hsvc, SERVICE_CONFIG_DESCRIPTION, &srvc_desc)) {
        CloseServiceHandle(hsvc);
        CloseServiceHandle(hmgr);
        convert_error_to_exception(GetLastError(), "service partially configured, error when defining the description");
        RETURN_THROWS();
    }
    if ((start_type & SERVICE_AUTO_START &&
         !ChangeServiceConfig2(hsvc, SERVICE_CONFIG_DELAYED_AUTO_START_INFO, &srvc_delayed_start))) {
        CloseServiceHandle(hsvc);
        CloseServiceHandle(hmgr);
        convert_error_to_exception(GetLastError(), "service partially configured, error on change the start type");
        RETURN_THROWS();
    }

    if (!ChangeServiceConfig2(hsvc, SERVICE_CONFIG_FAILURE_ACTIONS, &srvc_failure_infos)) {
        CloseServiceHandle(hsvc);
        CloseServiceHandle(hmgr);
        convert_error_to_exception(GetLastError(), "service partially configured, error on change the failure action");
        RETURN_THROWS();
    }

    if (!ChangeServiceConfig2(hsvc, SERVICE_CONFIG_FAILURE_ACTIONS_FLAG, &srvc_failure_action)) {
        CloseServiceHandle(hsvc);
        CloseServiceHandle(hmgr);
        convert_error_to_exception(GetLastError(),
                                   "service partially configured, error on change the failure action flag");
        RETURN_THROWS();
    }

    CloseServiceHandle(hsvc);
    CloseServiceHandle(hmgr);

    return;

    /* Store the base_priority in the registry. */
    spprintf(&service_key, 0, "%s%s", SERVICES_REG_KEY_ROOT, service);
    if (ERROR_SUCCESS != (registry_result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, service_key, 0, KEY_ALL_ACCESS, &hKey))) {
        convert_error_to_exception(registry_result, "On openning registry key to set base_priority");
        RETURN_THROWS();
    }
    if (ERROR_SUCCESS != (registry_result = RegSetValueEx(hKey, SERVICES_REG_BASE_PRIORITY, 0, REG_DWORD, (CONST
        BYTE *)&base_priority, sizeof(REG_DWORD)))) {
        convert_error_to_exception(registry_result, "On change the registry key value of base_priority");
        RETURN_THROWS();
    }

    RegCloseKey(hKey);
    efree(service_key);

}
/* }}} */

/* {{{ proto bool win32_exists_service(string servicename [, string machine])
   Chack if a service entry exists from the SCM database */
static PHP_FUNCTION(win32_exists_service) {
    char *machine = NULL;
    char *service = NULL;
    size_t machine_len = 0;
    size_t service_len = 0;
    SC_HANDLE hsvc;
    SC_HANDLE hmgr;
    long error = 0;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s|s!", &service, &service_len, &machine, &machine_len)) {
        RETURN_THROWS();
    }

    if (service_len == 0) {
        zend_argument_value_error(1, "the value cannot be empty");
        RETURN_THROWS();
    }

    hmgr = OpenSCManager(machine, NULL, SC_MANAGER_ALL_ACCESS);
    if (!hmgr) {
        convert_error_to_exception(GetLastError(), "");
        RETURN_THROWS();
    }
    hsvc = OpenService(hmgr, service, DELETE);
    if (!hsvc) {
        CloseServiceHandle(hmgr);
        error = GetLastError();
        if (error == ERROR_SERVICE_DOES_NOT_EXIST) {
            RETURN_FALSE;
        }
        convert_error_to_exception(error, "");
        RETURN_THROWS();
    }

    CloseServiceHandle(hsvc);
    CloseServiceHandle(hmgr);

    RETURN_TRUE;
}
/* }}} */

/* {{{ proto void win32_delete_service(string servicename [, string machine])
   Deletes a service entry from the SCM database */
static PHP_FUNCTION(win32_delete_service) {
    char *machine = NULL;
    char *service = NULL;
    size_t machine_len = 0;
    size_t service_len = 0;
    SC_HANDLE hsvc;
    SC_HANDLE hmgr;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s|s!", &service, &service_len, &machine, &machine_len)) {
        RETURN_THROWS();
    }

    if (service_len == 0) {
        zend_argument_value_error(1, "the value cannot be empty");
        RETURN_THROWS();
    }

    hmgr = OpenSCManager(machine, NULL, SC_MANAGER_ALL_ACCESS);
    if (!hmgr) {
        convert_error_to_exception(GetLastError(), "");
        RETURN_THROWS();
    }
    hsvc = OpenService(hmgr, service, DELETE);
    if (!hsvc) {
        CloseServiceHandle(hmgr);
        convert_error_to_exception(GetLastError(), "");
        RETURN_THROWS();
    }

    if (!DeleteService(hsvc)) {
        CloseServiceHandle(hmgr);
        CloseServiceHandle(hsvc);
        convert_error_to_exception(GetLastError(), "");
        RETURN_THROWS();
    }

    CloseServiceHandle(hsvc);
    CloseServiceHandle(hmgr);
}
/* }}} */

/* {{{ proto long win32_get_last_control_message()
   Returns the last control message that was sent to this service process */
static PHP_FUNCTION(win32_get_last_control_message) {
    if (win32_check_if_is_service() == FALSE && strcmp(sapi_module.name, "embed") != 0) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, 0,
                                "This function work only when using the CLI SAPI and called into the service code.");
        RETURN_THROWS();
    }

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_LONG(SVCG(args.dwControl));
}
/* }}} */

/* {{{ proto array win32_query_service_status(string servicename [, string machine])
   Queries the status of a service */
static PHP_FUNCTION(win32_query_service_status) {
    char *machine = NULL;
    char *service = NULL;
    size_t machine_len = 0;
    size_t service_len = 0;
    SC_HANDLE hsvc;
    SC_HANDLE hmgr;
    SERVICE_STATUS_PROCESS *st;
    DWORD size;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s|s!", &service, &service_len, &machine, &machine_len)) {
        RETURN_THROWS();
    }

    if (service_len == 0) {
        zend_argument_value_error(1, "the value cannot be empty");
        RETURN_THROWS();
    }

    hmgr = OpenSCManager(machine, NULL, GENERIC_READ);
    if (!hmgr) {
        convert_error_to_exception(GetLastError(), "");
        RETURN_THROWS();
    }
    hsvc = OpenService(hmgr, service, SERVICE_QUERY_STATUS);
    if (!hsvc) {
        CloseServiceHandle(hmgr);
        convert_error_to_exception(GetLastError(), "");
        RETURN_THROWS();
    }
    size = sizeof(*st);
    st = emalloc(size);
    if (!QueryServiceStatusEx(hsvc, SC_STATUS_PROCESS_INFO,
                              (LPBYTE) st, size, &size)) {
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            efree(st);
            CloseServiceHandle(hsvc);
            CloseServiceHandle(hmgr);
            convert_error_to_exception(GetLastError(), "");
            RETURN_THROWS();
        }
        st = erealloc(st, size);
        if (!QueryServiceStatusEx(hsvc, SC_STATUS_PROCESS_INFO,
                                  (LPBYTE) st, size, &size)) {
            efree(st);
            CloseServiceHandle(hsvc);
            CloseServiceHandle(hmgr);
            convert_error_to_exception(GetLastError(), "");
            RETURN_THROWS();
        }
    }
    /* map the struct to an array */
    array_init(return_value);
    add_assoc_long(return_value, "ServiceType", st->dwServiceType);
    add_assoc_long(return_value, "CurrentState", st->dwCurrentState);
    add_assoc_long(return_value, "ControlsAccepted", st->dwControlsAccepted);
    add_assoc_long(return_value, "Win32ExitCode", st->dwWin32ExitCode);
    add_assoc_long(return_value, "ServiceSpecificExitCode", st->dwServiceSpecificExitCode);
    add_assoc_long(return_value, "CheckPoint", st->dwCheckPoint);
    add_assoc_long(return_value, "WaitHint", st->dwWaitHint);
    add_assoc_long(return_value, "ProcessId", st->dwProcessId);
    add_assoc_long(return_value, "ServiceFlags", st->dwServiceFlags);

    efree(st);
    CloseServiceHandle(hsvc);
    CloseServiceHandle(hmgr);
}
/* }}} */

/* {{{ proto void win32_start_service(string servicename [, string machine])
   Starts a service */
static PHP_FUNCTION(win32_start_service) {
    char *machine = NULL;
    char *service = NULL;
    size_t machine_len = 0;
    size_t service_len = 0;
    SC_HANDLE hsvc;
    SC_HANDLE hmgr;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s|s!", &service, &service_len, &machine, &machine_len)) {
        RETURN_THROWS();
    }

    if (service_len == 0) {
        zend_argument_value_error(1, "the value cannot be empty");
        RETURN_THROWS();
    }

    hmgr = OpenSCManager(machine, NULL, SC_MANAGER_CONNECT);
    if (!hmgr) {
        convert_error_to_exception(GetLastError(), "");
        RETURN_THROWS();
    }
    hsvc = OpenService(hmgr, service, SERVICE_START);
    if (!hsvc) {
        CloseServiceHandle(hmgr);
        convert_error_to_exception(GetLastError(), "");
        RETURN_THROWS();
    }
    if (!StartService(hsvc, 0, NULL)) {
        CloseServiceHandle(hsvc);
        CloseServiceHandle(hmgr);
        convert_error_to_exception(GetLastError(), "");
        RETURN_THROWS();
    }
    CloseServiceHandle(hsvc);
    CloseServiceHandle(hmgr);
}

/* }}} */

/* {{{ proto void win32_get_service_env_vars(string servicename)
   Read all environment variables for local service. */
static PHP_FUNCTION(win32_get_service_env_vars) {
    char *service = NULL;
    size_t service_len = 0;
    HKEY hKey;
    LONG lReturnValue = 0;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s", &service, &service_len)) {
        RETURN_THROWS();
    }

    if (service_len == 0) {
        zend_argument_value_error(1, "the value cannot be empty");
        RETURN_THROWS();
    }
    array_init(return_value);
    char *pvData = NULL;
    int pvDataLen = 0;
    lReturnValue = get_service_environment_vars(service, service_len, &pvData, &pvDataLen);

    if (lReturnValue != ERROR_SUCCESS) {
        if (lReturnValue == ERROR_FILE_NOT_FOUND) {
            goto clearAndReturn;
        }

        convert_error_to_exception(lReturnValue, "Error on read service environment variables");
        RETURN_THROWS();
    }

    if (pvDataLen > 0) {

        int offset = 0;
        while (offset < pvDataLen) {
            const char *sub_string = pvData + offset;

            // Trouver la longueur de la sous-chaîne actuelle
            size_t sub_string_len = strlen(sub_string);

            // Si la sous-chaîne est vide, cela signifie que nous avons atteint le double null
            if (sub_string_len == 0) {
                break;
            }

            // Trouver le signe égal dans la sous-chaîne
            char *equal_sign = strchr(sub_string, '=');

            if (equal_sign != NULL) {
                // Extraire les deux parties de la sous-chaîne
                *equal_sign = '\0';  // Remplacer le '=' par un null pour séparer les deux parties
                const char *part1 = sub_string;
                const char *part2 = equal_sign + 1;

                add_assoc_string(return_value, part1, part2);

            } else {
                add_assoc_string(return_value, sub_string, "");
            }

            // Avancer à la prochaine sous-chaîne
            offset += sub_string_len + 1;
        }

    }

    clearAndReturn:
    efree(pvData);
}
/* }}} */

/* {{{ proto void win32_add_service_env_var(string servicename, string varName, string varValue)
   Add environment variable for local service. */
static PHP_FUNCTION(win32_add_service_env_var) {
    char *service = NULL;
    size_t service_len = 0;
    char *var_name = NULL;
    size_t var_name_len = 0;
    char *var_value = NULL;
    size_t var_value_len = 0;
    HKEY hKey, hSubkey;
    LONG lReturnValue = 0;
    DWORD dwDisposition;

    if (FAILURE ==
        zend_parse_parameters(ZEND_NUM_ARGS(), "sss", &service, &service_len, &var_name, &var_name_len, &var_value,
                              &var_value_len)) {
        RETURN_THROWS();
    }

    if (service_len == 0) {
        zend_argument_value_error(1, "the value cannot be empty");
        RETURN_THROWS();
    }

    if (var_name_len == 0) {
        zend_argument_value_error(2, "the value cannot be empty");
        RETURN_THROWS();
    }

    char *data = NULL;
    int data_len = 0;
    lReturnValue = get_service_environment_vars(service, service_len, &data, &data_len);

    if (lReturnValue != ERROR_SUCCESS) {
        if (lReturnValue == ERROR_SERVICE_DOES_NOT_EXIST) {
            convert_error_to_exception(ERROR_SERVICE_DOES_NOT_EXIST, service);
            RETURN_THROWS();
        }
        //Registry key does not exists now, but will be add after...
        if (lReturnValue == ERROR_FILE_NOT_FOUND) {
            goto continueAdd;
        }
        convert_error_to_exception(lReturnValue, "Error on read service environment variables");
        RETURN_THROWS();
    }

    continueAdd:

    char *new_data = NULL;
    int new_data_len = 0;
    add_or_replace_environment_value(data, data_len, var_name, var_value, &new_data, &new_data_len);


    if (data != NULL) {
        efree(data);
    }


    char *keyName = NULL;
    int keyNameLen = 0;
    get_service_registry_key(service, service_len, &keyName, &keyNameLen);

    lReturnValue = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyName, 0, KEY_ALL_ACCESS, &hKey);
    if (lReturnValue != ERROR_SUCCESS) {
        efree(keyName);
        efree(new_data);
        convert_error_to_exception(lReturnValue, "Error open key");
        RETURN_THROWS();
    }

    lReturnValue = RegSetValueEx(hKey, "Environment", 0, REG_MULTI_SZ, new_data, new_data_len);
    if (lReturnValue != ERROR_SUCCESS) {
        efree(keyName);
        efree(new_data);
        convert_error_to_exception(lReturnValue, "Error create key");
        RETURN_THROWS();
    }

    RegCloseKey(hKey);
    efree(keyName);
    efree(new_data);
    RETURN_NULL();
}
/* }}} */


/* {{{ proto void win32_remove_service_env_var(string servicename, string varName)
   Remove environment variable for local service. */
static PHP_FUNCTION(win32_remove_service_env_var) {
    char *service = NULL;
    size_t service_len = 0;
    char *var_name = NULL;
    size_t var_name_len = 0;
    HKEY hKey, hSubkey;
    LONG lReturnValue = 0;
    DWORD dwDisposition;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &service, &service_len, &var_name, &var_name_len)) {
        RETURN_THROWS();
    }

    if (service_len == 0) {
        zend_argument_value_error(1, "the value cannot be empty");
        RETURN_THROWS();
    }

    if (var_name_len == 0) {
        zend_argument_value_error(2, "the value cannot be empty");
        RETURN_THROWS();
    }

    char *data = NULL;
    int data_len = 0;
    lReturnValue = get_service_environment_vars(service, service_len, &data, &data_len);

    if (lReturnValue != ERROR_SUCCESS) {
        if (lReturnValue == ERROR_SERVICE_DOES_NOT_EXIST) {
            convert_error_to_exception(ERROR_SERVICE_DOES_NOT_EXIST, service);
            RETURN_THROWS();
        }

        convert_error_to_exception(lReturnValue, "Error on read service environment variables");
        RETURN_THROWS();
    }

    char *new_data = NULL;
    int new_data_len = 0;
    remove_environment_value(data, data_len, var_name, &new_data, &new_data_len);


    if (data != NULL) {
        efree(data);
    }


    char *keyName = NULL;
    int keyNameLen = 0;
    get_service_registry_key(service, service_len, &keyName, &keyNameLen);

    lReturnValue = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyName, 0, KEY_ALL_ACCESS, &hKey);
    if (lReturnValue != ERROR_SUCCESS) {
        efree(keyName);
        efree(new_data);
        convert_error_to_exception(lReturnValue, "Error open key");
        RETURN_THROWS();
    }
    if (new_data_len > 0) {
        lReturnValue = RegSetValueEx(hKey, "Environment", 0, REG_MULTI_SZ, new_data, new_data_len);
        if (lReturnValue != ERROR_SUCCESS) {
            efree(keyName);
            efree(new_data);
            convert_error_to_exception(lReturnValue, "Error create key");
            RETURN_THROWS();
        }
    } else {
        lReturnValue = RegDeleteValue(hKey, "Environment");
        if (lReturnValue != ERROR_SUCCESS) {
            efree(keyName);
            efree(new_data);
            convert_error_to_exception(lReturnValue, "Error remove key");
            RETURN_THROWS();
        }
    }
    RegCloseKey(hKey);
    efree(keyName);
    efree(new_data);
    RETURN_NULL();
}

/* }}} */


static void win32_handle_service_controls(INTERNAL_FUNCTION_PARAMETERS, long access, long status) /* {{{ */
{
    char *machine = NULL;
    char *service = NULL;
    size_t machine_len = 0;
    size_t service_len = 0;
    SC_HANDLE hsvc;
    SC_HANDLE hmgr;
    SERVICE_STATUS st;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s|s!", &service, &service_len, &machine, &machine_len)) {
        RETURN_THROWS();
    }

    if (service_len == 0) {
        zend_argument_value_error(1, "the value cannot be empty");
        RETURN_THROWS();
    }

    hmgr = OpenSCManager(machine, NULL, SC_MANAGER_CONNECT);
    if (!hmgr) {
        convert_error_to_exception(GetLastError(), "on openning manager");
        RETURN_THROWS();
    }
    hsvc = OpenService(hmgr, service, access);
    if (!hsvc) {
        CloseServiceHandle(hmgr);
        convert_error_to_exception(GetLastError(), "on openning service");
        RETURN_THROWS();
    }
    if (!ControlService(hsvc, status, &st)) {
        CloseServiceHandle(hsvc);
        CloseServiceHandle(hmgr);
        convert_error_to_exception(GetLastError(), "on send control");
        RETURN_THROWS();
    }
    CloseServiceHandle(hsvc);
    CloseServiceHandle(hmgr);
}

/* {{{ proto void win32_stop_service(string servicename [, string machine])
   Stops a service */
static PHP_FUNCTION(win32_stop_service) {
    win32_handle_service_controls(INTERNAL_FUNCTION_PARAM_PASSTHRU, SERVICE_STOP, SERVICE_CONTROL_STOP);
}
/* }}} */

/* {{{ proto void win32_pause_service(string servicename [, string machine])
   Pauses a service */
static PHP_FUNCTION(win32_pause_service) {
    win32_handle_service_controls(INTERNAL_FUNCTION_PARAM_PASSTHRU, SERVICE_PAUSE_CONTINUE, SERVICE_CONTROL_PAUSE);
}
/* }}} */

/* {{{ proto void win32_continue_service(string servicename [, string machine])
   Resumes a service */
static PHP_FUNCTION(win32_continue_service) {
    win32_handle_service_controls(INTERNAL_FUNCTION_PARAM_PASSTHRU, SERVICE_PAUSE_CONTINUE, SERVICE_CONTROL_CONTINUE);
}
/* }}} */


/* {{{ proto void win32_service_send_custom_control(string servicename, int control [, string machine])
   Stops a service */
static PHP_FUNCTION(win32_send_custom_control) {
    char *machine = NULL;
    char *service = NULL;
    long control;
    size_t machine_len = 0;
    size_t service_len = 0;
    SC_HANDLE hsvc;
    SC_HANDLE hmgr;
    SERVICE_STATUS st;

    if (FAILURE ==
        zend_parse_parameters(ZEND_NUM_ARGS(), "sl|s!", &service, &service_len, &control, &machine, &machine_len)) {
        RETURN_THROWS();
    }

    if (service_len == 0) {
        zend_argument_value_error(1, "the value cannot be empty");
        RETURN_THROWS();
    }

    if (control < 128 || control > 255) {
        zend_argument_value_error(2, "the value must be between 128 and 255. Got %d", control);
        RETURN_THROWS();
    }

    hmgr = OpenSCManager(machine, NULL, SC_MANAGER_CONNECT);
    if (!hmgr) {
        convert_error_to_exception(GetLastError(), "");
        RETURN_THROWS();
    }

    hsvc = OpenService(hmgr, service, SERVICE_USER_DEFINED_CONTROL);
    if (!hsvc) {
        CloseServiceHandle(hmgr);
        convert_error_to_exception(GetLastError(), "");
        RETURN_THROWS();
    }

    if (!ControlService(hsvc, control, &st)) {
        CloseServiceHandle(hsvc);
        CloseServiceHandle(hmgr);
        convert_error_to_exception(GetLastError(), "");
        RETURN_THROWS();
    }

    CloseServiceHandle(hsvc);
    CloseServiceHandle(hmgr);
}

/* }}} */

static int win32service_info_printf(const char *fmt, ...) /* {{{ */
{
    char *buf;
    size_t len, written;
    va_list argv;

    va_start(argv, fmt);
    len = vspprintf(&buf, 0, fmt, argv);
    va_end(argv);

    written = php_output_write(buf, len);
    efree(buf);
    return written;
}
/* }}} */

static zend_function_entry functions[] = {
        PHP_FE(win32_start_service_ctrl_dispatcher, arginfo_win32_start_service_ctrl_dispatcher)
        PHP_FE(win32_set_service_status, arginfo_win32_set_service_status)
        PHP_FE(win32_create_service, arginfo_win32_create_service)
        PHP_FE(win32_delete_service, arginfo_win32_delete_service)
        PHP_FE(win32_exists_service, arginfo_win32_exists_service)
        PHP_FE(win32_get_last_control_message, arginfo_win32_get_last_control_message)
        PHP_FE(win32_set_service_pause_resume_state, arginfo_win32_set_service_pause_resume_state)
        PHP_FE(win32_query_service_status, arginfo_win32_query_service_status)
        PHP_FE(win32_start_service, arginfo_win32_start_service)
        PHP_FE(win32_stop_service, arginfo_win32_stop_service)
        PHP_FE(win32_pause_service, arginfo_win32_pause_service)
        PHP_FE(win32_continue_service, arginfo_win32_continue_service)
        PHP_FE(win32_set_service_exit_mode, arginfo_win32_set_service_exit_mode)
        PHP_FE(win32_set_service_exit_code, arginfo_win32_set_service_exit_code)
        PHP_FE(win32_send_custom_control, arginfo_win32_send_custom_control)
        PHP_FE(win32_add_right_access_service, arginfo_win32_add_right_access_service)
        PHP_FE(win32_remove_right_access_service, arginfo_win32_remove_right_access_service)
        PHP_FE(win32_read_right_access_service, arginfo_win32_read_right_access_service)
        PHP_FE(win32_read_all_rights_access_service, arginfo_win32_read_all_rights_access_service)
        PHP_FE(win32_add_service_env_var, arginfo_win32_add_service_env_var)
        PHP_FE(win32_remove_service_env_var, arginfo_win32_remove_service_env_var)
        PHP_FE(win32_get_service_env_vars, arginfo_win32_get_service_env_vars)
        PHP_FE_END
};

static void init_globals(zend_win32service_globals *g) {
    memset(g, 0, sizeof(*g));
    g->gracefulExit = 1;
    g->exitCode = 1;
    g->dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE |
                            SERVICE_ACCEPT_HARDWAREPROFILECHANGE | SERVICE_ACCEPT_NETBINDCHANGE |
                            SERVICE_ACCEPT_PARAMCHANGE | SERVICE_ACCEPT_POWEREVENT | SERVICE_ACCEPT_SESSIONCHANGE |
                            SERVICE_ACCEPT_PRESHUTDOWN | SERVICE_ACCEPT_TIMECHANGE | SERVICE_ACCEPT_TRIGGEREVENT;
}

void win32service_register_Win32ServiceException_class(void) {
    zend_class_entry ce;

    /* Create and register 'Win32ServiceException' class. */
    INIT_CLASS_ENTRY_EX(ce, "Win32ServiceException", sizeof("Win32ServiceException") - 1, NULL);
    Win32ServiceException_ce_ptr = zend_register_internal_class_ex(&ce,
                                                                   zend_ce_exception);
    Win32ServiceException_ce_ptr->create_object = zend_ce_exception->create_object;
}

static PHP_MINIT_FUNCTION(win32service) {

    ZEND_INIT_MODULE_GLOBALS(win32service, init_globals, NULL);
    win32service_register_Win32ServiceException_class();

    PHP_MINIT(win32service_right_info)(INIT_FUNC_ARGS_PASSTHRU);

#define MKCONST(x)    REGISTER_LONG_CONSTANT("WIN32_" # x, x, CONST_CS|CONST_PERSISTENT)
#define MKSCONST(x)    REGISTER_STRING_CONSTANT("WIN32_" # x, x, CONST_CS|CONST_PERSISTENT)

    /* Constants used in communication with the SCM */

    /* dwServiceType */
    /* MKCONST(SERVICE_KERNEL_DRIVER);                 /* 0x00000001 Driver service. */
    /* MKCONST(SERVICE_FILE_SYSTEM_DRIVER);            /* 0x00000002 File system driver service. */
    /* MKCONST(SERVICE_ADAPTER);                       /* 0x00000004 Reserved. */
    /* MKCONST(SERVICE_RECOGNIZER_DRIVER);             /* 0x00000008 Reserved. */
    MKCONST(SERVICE_WIN32_OWN_PROCESS);                /* 0x00000010 Service that runs in its own process. */
    /* MKCONST(SERVICE_WIN32_SHARE_PROCESS);           /* 0x00000020 Service that shares a process with one or more other services. */
    MKCONST(SERVICE_INTERACTIVE_PROCESS);              /* 0x00000100 The service can interact with the desktop. */
    MKCONST(SERVICE_WIN32_OWN_PROCESS_INTERACTIVE);    /* 0x00000110 Service that runs in its own process and interacts with the desktop. */

    /* dwCurrentState */
    MKCONST(SERVICE_CONTINUE_PENDING);                 /* 0x00000005 The service continue is pending. */
    MKCONST(SERVICE_PAUSE_PENDING);                    /* 0x00000006 The service pause is pending. */
    MKCONST(SERVICE_PAUSED);                           /* 0x00000007 The service is paused. */
    MKCONST(SERVICE_RUNNING);                          /* 0x00000004 The service is running. */
    MKCONST(SERVICE_START_PENDING);                    /* 0x00000002 The service is starting. */
    MKCONST(SERVICE_STOP_PENDING);                     /* 0x00000003 The service is stopping. */
    MKCONST(SERVICE_STOPPED);                          /* 0x00000001 The service is not running. */

    /* dwControl */
    MKCONST(SERVICE_CONTROL_CONTINUE);                 /* 0x00000003 Notifies a paused service that it should resume. */
    MKCONST(SERVICE_CONTROL_DEVICEEVENT);              /* 0x0000000B */
    MKCONST(SERVICE_CONTROL_HARDWAREPROFILECHANGE);    /* 0x0000000C */
    MKCONST(SERVICE_CONTROL_INTERROGATE);              /* 0x00000004 Notifies a service that it should report its current status information to the service control manager. */
    MKCONST(SERVICE_CONTROL_NETBINDADD);               /* 0x00000007 Notifies a network service that there is a new component for binding. */
    MKCONST(SERVICE_CONTROL_NETBINDDISABLE);           /* 0x0000000A Notifies a network service that one of its bindings has been disabled. */
    MKCONST(SERVICE_CONTROL_NETBINDENABLE);            /* 0x00000009 Notifies a network service that a disabled binding has been enabled. */
    MKCONST(SERVICE_CONTROL_NETBINDREMOVE);            /* 0x00000008 Notifies a network service that a component for binding has been removed. */
    MKCONST(SERVICE_CONTROL_PARAMCHANGE);              /* 0x00000006 Notifies a service that its startup parameters have changed. */
    MKCONST(SERVICE_CONTROL_PAUSE);                    /* 0x00000002 Notifies a service that it should pause. */
    MKCONST(SERVICE_CONTROL_POWEREVENT);               /* 0x0000000D */
    MKCONST(SERVICE_CONTROL_PRESHUTDOWN);              /* 0x0000000F Notifies a service that the system will be shutting down.
	                                                                 Services that need additional time to perform cleanup tasks beyond the tight time restriction at system shutdown can use this notification.
	                                                                 The service control manager sends this notification to applications that have registered for it before sending a SERVICE_CONTROL_SHUTDOWN notification to applications that have registered for that notification.
	                                                                 A service that handles this notification blocks system shutdown until the service stops or the preshutdown time-out interval specified through SERVICE_PRESHUTDOWN_INFO expires.
	                                                                 Because this affects the user experience, services should use this feature only if it is absolutely necessary to avoid data loss or significant recovery time at the next system start.
	                                                                 Windows Server 2003 and Windows XP/2000:  This value is not supported. */
    MKCONST(SERVICE_CONTROL_SESSIONCHANGE);            /* 0x0000000E */
    MKCONST(SERVICE_CONTROL_SHUTDOWN);                 /* 0x00000005 Notifies a service that the system is shutting down so the service can perform cleanup tasks.
	                                                                 Note that services that register for SERVICE_CONTROL_PRESHUTDOWN notifications cannot receive this notification because they have already stopped.
	                                                                 If a service accepts this control code, it must stop after it performs its cleanup tasks and return NO_ERROR.
	                                                                 After the SCM sends this control code, it will not send other control codes to the service. */
    MKCONST(SERVICE_CONTROL_STOP);                     /* 0x00000001 Notifies a service that it should stop. */

    /* dwControlsAccepted */
    MKCONST(SERVICE_ACCEPT_HARDWAREPROFILECHANGE);     /* 0x00000020 The service is notified when the computer's hardware profile has changed.
	                                                                 This enables the system to send SERVICE_CONTROL_HARDWAREPROFILECHANGE notifications to the service. */
    MKCONST(SERVICE_ACCEPT_NETBINDCHANGE);             /* 0x00000010 The service is a network component that can accept changes in its binding without being stopped and restarted.
	                                                                 This control code allows the service to receive SERVICE_CONTROL_NETBINDADD, SERVICE_CONTROL_NETBINDREMOVE, SERVICE_CONTROL_NETBINDENABLE, and SERVICE_CONTROL_NETBINDDISABLE notifications. */
    MKCONST(SERVICE_ACCEPT_PARAMCHANGE);               /* 0x00000008 The service can reread its startup parameters without being stopped and restarted.
	                                                                 This control code allows the service to receive SERVICE_CONTROL_PARAMCHANGE notifications. */
    MKCONST(SERVICE_ACCEPT_PAUSE_CONTINUE);            /* 0x00000002 The service can be paused and continued.
	                                                                 This control code allows the service to receive SERVICE_CONTROL_PAUSE and SERVICE_CONTROL_CONTINUE notifications. */
    MKCONST(SERVICE_ACCEPT_POWEREVENT);                /* 0x00000040 The service is notified when the computer's power status has changed.
	                                                                 This enables the system to send SERVICE_CONTROL_POWEREVENT notifications to the service. */
    MKCONST(SERVICE_ACCEPT_PRESHUTDOWN);               /* 0x00000100 The service can perform preshutdown tasks.
	                                                                 This control code enables the service to receive SERVICE_CONTROL_PRESHUTDOWN notifications.
	                                                                 Note that ControlService and ControlServiceEx cannot send this notification; only the system can send it.
	                                                                 Windows Server 2003 and Windows XP/2000:  This value is not supported. */
    MKCONST(SERVICE_ACCEPT_SESSIONCHANGE);             /* 0x00000080 The service is notified when the computer's session status has changed.
	                                                                 This enables the system to send SERVICE_CONTROL_SESSIONCHANGE notifications to the service.
	                                                                 Windows 2000:  This value is not supported. */
    MKCONST(SERVICE_ACCEPT_SHUTDOWN);                  /* 0x00000004 The service is notified when system shutdown occurs.
	                                                                 This control code allows the service to receive SERVICE_CONTROL_SHUTDOWN notifications.
	                                                                 Note that ControlService and ControlServiceEx cannot send this notification; only the system can send it. */
    MKCONST(SERVICE_ACCEPT_STOP);                      /* 0x00000001 The service can be stopped.
	                                                                 This control code allows the service to receive SERVICE_CONTROL_STOP notifications. */
    MKCONST(SERVICE_ACCEPT_TIMECHANGE);                /* 0x00000200 The service is notified when the system time has changed.
	                                                                 This enables the system to send SERVICE_CONTROL_TIMECHANGE notifications to the service.
	                                                                 Windows Server 2008, Windows Vista, Windows Server 2003, and Windows XP/2000:  This control code is not supported. */
    MKCONST(SERVICE_ACCEPT_TRIGGEREVENT);              /* 0x00000400 The service is notified when an event for which the service has registered occurs.
	                                                                 This enables the system to send SERVICE_CONTROL_TRIGGEREVENT notifications to the service.
	                                                                 Windows Server 2008, Windows Vista, Windows Server 2003, and Windows XP/2000:  This control code is not supported. */

    /* dwStartType */
    MKCONST(SERVICE_BOOT_START);                       /* 0x00000000 A device driver started by the system loader. This value is valid only for driver services. */
    MKCONST(SERVICE_SYSTEM_START);                     /* 0x00000001 A device driver started by the IoInitSystem function. This value is valid only for driver services. */
    MKCONST(SERVICE_AUTO_START);                       /* 0x00000002 A service started automatically by the service control manager during system startup. */
    MKCONST(SERVICE_DEMAND_START);                     /* 0x00000003 A service started by the service control manager when a process calls the StartService function. */
    MKCONST(SERVICE_DISABLED);                         /* 0x00000004 A service that cannot be started. Attempts to start the service result in the error code ERROR_SERVICE_DISABLED. */

    /* dwErrorControl */
    MKCONST(SERVICE_ERROR_IGNORE);                     /* 0x00000000 The startup program ignores the error and continues the startup operation. */
    MKCONST(SERVICE_ERROR_NORMAL);                     /* 0x00000001 The startup program logs the error in the event log but continues the startup operation. */
    MKCONST(SERVICE_ERROR_SEVERE);                     /* 0x00000002 The startup program logs the error in the event log.
	                                                                 If the last-known-good configuration is being started, the startup operation continues. Otherwise, the system is restarted with the last-known-good configuration. */
    MKCONST(SERVICE_ERROR_CRITICAL);                   /* 0x00000003 The startup program logs the error in the event log, if possible.
	                                                                 If the last-known-good configuration is being started, the startup operation fails. Otherwise, the system is restarted with the last-known good configuration. */

    /* dwServiceFlags */
    MKCONST(SERVICE_RUNS_IN_SYSTEM_PROCESS);           /* 0x00000001 The service runs in a system process that must always be running. */

    /* Error constants generated in communication with the SCM */
    MKCONST(ERROR_ACCESS_DENIED);                      /* 0x00000005 The handle to the SCM database does not have the appropriate access rights. */
    MKCONST(ERROR_CIRCULAR_DEPENDENCY);                /* 0x00000423 A circular service dependency was specified. */
    MKCONST(ERROR_DATABASE_DOES_NOT_EXIST);            /* 0x00000429 The specified database does not exist. */
    MKCONST(ERROR_DEPENDENT_SERVICES_RUNNING);         /* 0x0000041B The service cannot be stopped because other running services are dependent on it. */
    MKCONST(ERROR_DUPLICATE_SERVICE_NAME);             /* 0x00000436 The display name already exists in the service control manager database either as a service name or as another display name. */
    MKCONST(ERROR_FAILED_SERVICE_CONTROLLER_CONNECT);  /* 0x00000427 The service process could not connect to the service controller. */
    MKCONST(ERROR_INSUFFICIENT_BUFFER);                /* 0x0000007A The buffer is too small for the SERVICE_STATUS_PROCESS structure. Nothing was written to the structure. */
    MKCONST(ERROR_INVALID_DATA);                       /* 0x0000000D The specified service status structure is invalid. */
    MKCONST(ERROR_INVALID_HANDLE);                     /* 0x00000006 The handle to the specified service control manager database is invalid. */
    MKCONST(ERROR_INVALID_LEVEL);                      /* 0x0000007C The InfoLevel parameter contains an unsupported value. */
    MKCONST(ERROR_INVALID_NAME);                       /* 0x0000007B The specified service name is invalid. */
    MKCONST(ERROR_INVALID_PARAMETER);                  /* 0x00000057 A parameter that was specified is invalid (CreateService) or the cbSize member of SERVICE_STATUS_PROCESS is not valid (QueryServiceStatusEx). */
    MKCONST(ERROR_INVALID_SERVICE_ACCOUNT);            /* 0x00000421 The user account name specified in the lpServiceStartName parameter does not exist. */
    MKCONST(ERROR_INVALID_SERVICE_CONTROL);            /* 0x0000041C The requested control code is not valid, or it is unacceptable to the service. */
    MKCONST(ERROR_PATH_NOT_FOUND);                     /* 0x00000003 The service binary file could not be found. */
    MKCONST(ERROR_SERVICE_ALREADY_RUNNING);            /* 0x00000420 An instance of the service is already running. */
    MKCONST(ERROR_SERVICE_CANNOT_ACCEPT_CTRL);         /* 0x00000425 The requested control code cannot be sent to the service because the state of the service is SERVICE_STOPPED, SERVICE_START_PENDING, or SERVICE_STOP_PENDING. */
    MKCONST(ERROR_SERVICE_DATABASE_LOCKED);            /* 0x0000041F The database is locked. */
    MKCONST(ERROR_SERVICE_DEPENDENCY_DELETED);         /* 0x00000433 The service depends on a service that does not exist or has been marked for deletion. */
    MKCONST(ERROR_SERVICE_DEPENDENCY_FAIL);            /* 0x0000042C The service depends on another service that has failed to start. */
    MKCONST(ERROR_SERVICE_DISABLED);                   /* 0x00000422 The service has been disabled. */
    MKCONST(ERROR_SERVICE_DOES_NOT_EXIST);             /* 0x00000424 The specified service does not exist as an installed service. */
    MKCONST(ERROR_SERVICE_EXISTS);                     /* 0x00000431 The specified service already exists in this database. */
    MKCONST(ERROR_SERVICE_LOGON_FAILED);               /* 0x0000042D The service did not start due to a logon failure. This error occurs if the service is configured to run under an account that does not have the "Log on as a service" right. */
    MKCONST(ERROR_SERVICE_MARKED_FOR_DELETE);          /* 0x00000430 The specified service has already been marked for deletion. */
    MKCONST(ERROR_SERVICE_NO_THREAD);                  /* 0x0000041E A thread could not be created for the service. */
    MKCONST(ERROR_SERVICE_NOT_ACTIVE);                 /* 0x00000426 The service has not been started. */
    MKCONST(ERROR_SERVICE_REQUEST_TIMEOUT);            /* 0x0000041D The process for the service was started, but it did not call StartServiceCtrlDispatcher, or the thread that called StartServiceCtrlDispatcher may be blocked in a control handler function. */
    MKCONST(ERROR_SHUTDOWN_IN_PROGRESS);               /* 0x0000045B The system is shutting down; this function cannot be called. */
    MKCONST(ERROR_SERVICE_SPECIFIC_ERROR);             /* 0x0000042A The service has returned a service-specific error code. */
    MKCONST(NO_ERROR);                                 /* 0x00000000 No error. */

    /* Win32 Priority Constants */
    MKCONST(ABOVE_NORMAL_PRIORITY_CLASS);              /* 0x00008000 Process that has priority above NORMAL_PRIORITY_CLASS but below HIGH_PRIORITY_CLASS. */
    MKCONST(BELOW_NORMAL_PRIORITY_CLASS);              /* 0x00004000 Process that has priority above IDLE_PRIORITY_CLASS but below NORMAL_PRIORITY_CLASS. */
    MKCONST(HIGH_PRIORITY_CLASS);                      /* 0x00000080 Process that performs time-critical tasks that must be executed immediately. The threads of the process preempt the threads of normal or idle priority class processes. An example is the Task List, which must respond quickly when called by the user, regardless of the load on the operating system. Use extreme care when using the high-priority class, because a high-priority class application can use nearly all available CPU time. */
    MKCONST(IDLE_PRIORITY_CLASS);                      /* 0x00000040 Process whose threads run only when the system is idle. The threads of the process are preempted by the threads of any process running in a higher priority class. An example is a screen saver. The idle-priority class is inherited by child processes. */
    MKCONST(NORMAL_PRIORITY_CLASS);                    /* 0x00000020 Process with no special scheduling needs. */
    MKCONST(REALTIME_PRIORITY_CLASS);                  /* 0x00000100 Process that has the highest possible priority. The threads of the process preempt the threads of all other processes, including operating system processes performing important tasks. For example, a real-time process that executes for more than a very brief interval can cause disk caches not to flush or cause the mouse to be unresponsive. */

    /* Win32 Recovery Constants */
    MKCONST(SC_ACTION_NONE);                            /* 0x00000000 No Action */
    MKCONST(SC_ACTION_RESTART);                            /* 0x00000001 Restart the service */
    MKCONST(SC_ACTION_REBOOT);                            /* 0x00000002 Reboot the computer */
    MKCONST(SC_ACTION_RUN_COMMAND);                        /* 0x00000003 Run the command */

    /* Win32 Service Right Access Constants  https://learn.microsoft.com/fr-fr/windows/win32/services/service-security-and-access-rights?source=recommendations */
    MKCONST(SERVICE_ALL_ACCESS);                            /* 0x000F01FF Full access + STANDARD_RIGHTS_REQUIRED */
    MKCONST(SERVICE_CHANGE_CONFIG);                        /* 0x00000002 Needed to change the service configuration (administrator only) */
    MKCONST(SERVICE_ENUMERATE_DEPENDENTS);        /* 0x00000008 Needed to get the dependency list for a service */
    MKCONST(SERVICE_INTERROGATE);                            /* 0x00000080 Needed to request the service to send status */
    MKCONST(SERVICE_PAUSE_CONTINUE);                    /* 0x00000040 Needed to pause/resume a service */
    MKCONST(SERVICE_QUERY_CONFIG);                        /* 0x00000001 Needed to read the service configuration */
    MKCONST(SERVICE_QUERY_STATUS);                        /* 0x00000004 Needed to request the service status */
    MKCONST(SERVICE_START);                                        /* 0x00000010 Needed to start service */
    MKCONST(SERVICE_STOP);                                /* 0x00000020 Needed to stop service */
    MKCONST(SERVICE_USER_DEFINED_CONTROL);      /* 0x00000100 Needed to send custom user control to service */
    MKCONST(ACCESS_SYSTEM_SECURITY);                    /* 0xxxxxxxxx Needed to access to the SACL  */
    MKCONST(DELETE);                                                    /* 0x00010000 Needed to delete a service */
    MKCONST(READ_CONTROL);                                        /* 0x00020000 Needed to read the security descriptor */
    MKCONST(WRITE_DAC);                                            /* 0x00040000 Needed to write the security descriptor */
    MKCONST(WRITE_OWNER);                                        /* 0x00080000 Needed to change the owner and group of security descriptor */

    MKCONST(GENERIC_READ);                                        /* Right sum of STANDARD_RIGHTS_READ
																														SERVICE_QUERY_CONFIG
																														SERVICE_QUERY_STATUS
																														SERVICE_INTERROGATE
																														SERVICE_ENUMERATE_DEPENDENTS */
    MKCONST(GENERIC_WRITE);                                    /* Right sum of STANDARD_RIGHTS_WRITE
                                                          	SERVICE_CHANGE_CONFIG */
    MKCONST(GENERIC_EXECUTE);                                /* Right sum of STANDARD_RIGHTS_EXECUTE
                                                            SERVICE_START
                                                            SERVICE_STOP
                                                            SERVICE_PAUSE_CONTINUE
                                                            SERVICE_USER_DEFINED_CONTROL */


    /* Win32 Service informations */
    MKSCONST(INFO_SERVICE);
    MKSCONST(INFO_DISPLAY);
    MKSCONST(INFO_USER);
    MKSCONST(INFO_PASSWORD);
    MKSCONST(INFO_PATH);
    MKSCONST(INFO_PARAMS);
    MKSCONST(INFO_DESCRIPTION);
    MKSCONST(INFO_START_TYPE);
    MKSCONST(INFO_LOAD_ORDER);
    MKSCONST(INFO_SVC_TYPE);
    MKSCONST(INFO_ERROR_CONTROL);
    MKSCONST(INFO_DELAYED_START);
    MKSCONST(INFO_BASE_PRIORITY);
    MKSCONST(INFO_DEPENDENCIES);
    MKSCONST(INFO_RECOVERY_DELAY);
    MKSCONST(INFO_RECOVERY_ACTION_1);
    MKSCONST(INFO_RECOVERY_ACTION_2);
    MKSCONST(INFO_RECOVERY_ACTION_3);
    MKSCONST(INFO_RECOVERY_RESET_PERIOD);
    MKSCONST(INFO_RECOVERY_ENABLED);
    MKSCONST(INFO_RECOVERY_REBOOT_MSG);
    MKSCONST(INFO_RECOVERY_COMMAND);

    return SUCCESS;
}

static PHP_RSHUTDOWN_FUNCTION(win32service) {
    if (SVCG(sh) && SVCG(st).dwCurrentState != SERVICE_STOPPED) {
        zend_bool graceful = SVCG(gracefulExit);
        /* Log the mode and code used for exit in PHP log */
        char *str = emalloc(sizeof(char) * 150);

        if (graceful == 0) {
            SVCG(st).dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
            SVCG(st).dwServiceSpecificExitCode = SVCG(exitCode);
            sprintf(str, "Info: Win32Service exit with error and exit code %l", SVCG(exitCode));
        } else {
            sprintf(str, "Info: Win32Service exit gracefuly");
        }

        php_log_err(str);
        efree(str);

        SVCG(st).dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(SVCG(sh), &SVCG(st));
        /* PostThreadMessage(SVCG(svc_thread_id), WM_QUIT, 0, 0); */
    }
    if (SVCG(svc_thread)) {
        WaitForSingleObject(SVCG(svc_thread), 10000);
        CloseHandle(SVCG(svc_thread));
    }
    if (SVCG(event)) {
        CloseHandle(SVCG(event));
    }
    if (SVCG(service_name)) {
        efree(SVCG(service_name));
    }
    return SUCCESS;
}

static PHP_MINFO_FUNCTION(win32service) {
    php_info_print_table_start();

    if (!sapi_module.phpinfo_as_text) {
        win32service_info_printf("<tr><td class=\"v\">%s</td></tr>\n",
                                 "The maintainer needs your feedback (good or bad), please send it to: <a href=\"mailto:win32service@mactronique.fr\">win32service@mactronique.fr</a>");
        win32service_info_printf("<tr><td class=\"v\">%s</td></tr>\n",
                                 "Please report your bugs with this extension here : <a href=\"https://github.com/win32service/win32service/issues\"  target=\"bugreport\">Win32Service PHP extension issue tracker</a>");
        win32service_info_printf("<tr><td class=\"v\">%s</td></tr>\n",
                                 "Home page: <a href=\"http://win32service.mactronique.fr/\" target=\"mactronique\">http://win32service.mactronique.fr/</a>");
        win32service_info_printf("<tr><td class=\"v\">%s</td></tr>\n",
                                 "Library for help you to use this extension: <a href=\"https://github.com/win32service/service-library\" target=\"libraire\">service-library</a> and <a href=\"https://github.com/win32service/Win32ServiceBundle\" target=\"bundle\">Win32ServiceBundle</a> for Symfony");
    } else {
        php_info_print_table_row(1,
                                 "The maintainer needs your feedback (good or bad), please send it to: win32service@mactronique.fr");
        php_info_print_table_row(1, "Please report your bugs with this extension here : https://github.com/win32service/win32service/issues/");
        php_info_print_table_row(1, "Home page: http://win32service.mactronique.fr/");
        php_info_print_table_row(1,
                                 "Library for help you to use this extension: https://github.com/win32service/service-library and https://github.com/win32service/Win32ServiceBundle for Symfony");
    }
    php_info_print_table_end();

    php_info_print_table_start();

    php_info_print_table_row(2, "Win32 Service support", "enabled");
    php_info_print_table_row(2, "Version", PHP_WIN32SERVICE_VERSION);
    php_info_print_table_row(2, "Current SAPI", sapi_module.name);
    php_info_print_table_row(2, "Run in Windows Service context", win32_check_if_is_service() ? "true":"false. Some functions are disabled.");
    if (!sapi_module.phpinfo_as_text) {
        php_info_print_table_row(2, "Security information",
                                 "The Win32Service extension does work when using the CLI SAPI with administrator right level. On other SAPI, please <a href=\"https://www.php.net/manual/en/win32service.security.php\">check security consideration</a>.");
    } else {
        php_info_print_table_row(2, "Security information",
                                 "The Win32Service extension does work when using the CLI SAPI with administrator right level. On other SAPI, please check security consideration: https://www.php.net/manual/en/win32service.security.php");
    }
    php_info_print_table_end();

    php_info_print_table_start();
    php_info_print_table_header(2, "Function", "State for the current SAPI");
    if (win32_check_if_is_service() == TRUE || !strcmp(sapi_module.name, "embed")) {
        php_info_print_table_row(2, "win32_start_service_ctrl_dispatcher", "enabled");
        php_info_print_table_row(2, "win32_set_service_status", "enabled");
        php_info_print_table_row(2, "win32_get_last_control_message", "enabled");
        php_info_print_table_row(2, "win32_set_service_exit_mode", "enabled");
        php_info_print_table_row(2, "win32_set_service_exit_code", "enabled");
        php_info_print_table_row(2, "win32_set_service_pause_resume_state", "enabled");
    } else {
        php_info_print_table_row(2, "win32_start_service_ctrl_dispatcher", "disabled");
        php_info_print_table_row(2, "win32_set_service_status", "disabled");
        php_info_print_table_row(2, "win32_get_last_control_message", "disabled");
        php_info_print_table_row(2, "win32_set_service_exit_mode", "disabled");
        php_info_print_table_row(2, "win32_set_service_exit_code", "disabled");
        php_info_print_table_row(2, "win32_set_service_pause_resume_state", "disabled");
    }
    php_info_print_table_row(2, "win32_create_service", "enabled");
    php_info_print_table_row(2, "win32_delete_service", "enabled");
    php_info_print_table_row(2, "win32_query_service_status", "enabled");
    php_info_print_table_row(2, "win32_start_service", "enabled");
    php_info_print_table_row(2, "win32_stop_service", "enabled");
    php_info_print_table_row(2, "win32_pause_service", "enabled");
    php_info_print_table_row(2, "win32_continue_service", "enabled");
    php_info_print_table_row(2, "win32_send_custom_control", "enabled");
    php_info_print_table_row(2, "win32_read_right_access_service", "enabled");
    php_info_print_table_row(2, "win32_read_all_rights_access_service", "enabled");
    php_info_print_table_row(2, "win32_add_right_access_service", "enabled");
    php_info_print_table_row(2, "win32_remove_right_access_service", "enabled");
    php_info_print_table_row(2, "win32_add_service_env_var", "enabled");
    php_info_print_table_row(2, "win32_remove_service_env_var", "enabled");
    php_info_print_table_row(2, "win32_get_service_env_vars", "enabled");
    php_info_print_table_end();
}

zend_module_entry win32service_module_entry = {
        STANDARD_MODULE_HEADER,
        "win32service",
        functions,
        PHP_MINIT(win32service),
        NULL,
        NULL,
        PHP_RSHUTDOWN(win32service),
        PHP_MINFO(win32service),
        PHP_WIN32SERVICE_VERSION,
        STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_WIN32SERVICE
ZEND_GET_MODULE(win32service)
#endif
