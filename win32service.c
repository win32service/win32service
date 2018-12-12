/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2011 The PHP Group                                |
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
  | Maintainer: Jean-Baptiste Nahan <jb@nahan.fr>                        |
  +----------------------------------------------------------------------+
*/

/* $Id: win32service.c 313721 2011-07-26 11:46:19Z rquadling $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_win32service.h"
#include "php_win32service_int.h"
#include "SAPI.h"

#define SERVICES_REG_BASE_PRIORITY "BasePriority"
#define SERVICES_REG_KEY_ROOT "SYSTEM\\CurrentControlSet\\Services\\"

/* gargh! service_main run from a new thread that we don't spawn, so we can't do this nicely */
static void *tmp_service_g = NULL;

static DWORD WINAPI service_handler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
	zend_win32service_globals *g = (zend_win32service_globals*)lpContext;
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

static void WINAPI service_main(DWORD argc, char **argv)
{
	zend_win32service_globals *g = (zend_win32service_globals*)tmp_service_g;
	OSVERSIONINFO osvi;
	DWORD base_priority;
	HKEY hKey;
	char *service_key;
	long registry_result = ERROR_SUCCESS;
	DWORD dwType = REG_DWORD;
	DWORD dwSize = sizeof(DWORD);

	/* Get the current OS version. */
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

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
	//g->st.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE | (osvi.dwMajorVersion >= 6 ? SERVICE_ACCEPT_PRESHUTDOWN : 0); /* Allow the service to be paused and handle Vista-style pre-shutdown notifications. */
    g->st.dwControlsAccepted =  SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE |
						        SERVICE_ACCEPT_HARDWAREPROFILECHANGE | SERVICE_ACCEPT_NETBINDCHANGE |
								SERVICE_ACCEPT_PARAMCHANGE | SERVICE_ACCEPT_POWEREVENT;

	//XP and newer Accepts
    if ( !(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 ) ) {
        g->st.dwControlsAccepted |= SERVICE_ACCEPT_SESSIONCHANGE;
    }
    //Vista and newer Accepts
    if (osvi.dwMajorVersion >= 6) {
        g->st.dwControlsAccepted |= SERVICE_ACCEPT_PRESHUTDOWN;
    }
    //Windows Server 2008, Windows Vista, Windows Server 2003, and Windows XP/2000: This control code is not supported.
    if ( !(osvi.dwMajorVersion == 5) && !(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 ) ) {
        g->st.dwControlsAccepted |= SERVICE_ACCEPT_TIMECHANGE | SERVICE_ACCEPT_TRIGGEREVENT;
    }
	g->sh = RegisterServiceCtrlHandlerEx(g->service_name, service_handler, g);

	if (g->sh == (SERVICE_STATUS_HANDLE)0) {
		g->code = GetLastError();
		SetEvent(g->event);
		return;
	}

	g->code = NO_ERROR;
	SetEvent(g->event);
}

static DWORD WINAPI svc_thread_proc(LPVOID _globals)
{
	zend_win32service_globals *g = (zend_win32service_globals*)_globals;

	tmp_service_g = g;

	if (!StartServiceCtrlDispatcher(g->te)) {
		g->code = GetLastError();
		SetEvent(g->event);
		return 1;
	}

	/* not reached until service_main returns */
	return 0;
}

/* {{{ proto bool win32_start_service_ctrl_dispatcher(string $name)
   Registers the script with the SCM, so that it can act as the service with the given name */
static PHP_FUNCTION(win32_start_service_ctrl_dispatcher)
{
	if (strcmp(sapi_module.name, "cli") != 0) {
		zend_error(E_ERROR, "This function work only when using the CLI SAPI and called into the service code.");
		RETURN_FALSE;
	}

	char *name;
	size_t name_len;

	if (SVCG(svc_thread)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "service ctrl dispatcher already running");
		RETURN_FALSE;
	}

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len)) {
		RETURN_FALSE;
	}

	SVCG(service_name) = estrdup(name);

	SVCG(te)[0].lpServiceName = SVCG(service_name);
	SVCG(te)[0].lpServiceProc = service_main;
	SVCG(event) = CreateEvent(NULL, TRUE, FALSE, NULL);

	SVCG(svc_thread) = CreateThread(NULL, 0, svc_thread_proc, &SVCG(svc_thread), 0, &SVCG(svc_thread_id));

	if (SVCG(svc_thread) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to start dispatcher thread");
		RETURN_FALSE;
	}

	if (WAIT_OBJECT_0 == WaitForSingleObject(SVCG(event), 30000)) {
		if (SVCG(code) == NO_ERROR) {
			RETURN_TRUE;
		} else {
			RETURN_LONG(SVCG(code));
		}
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool win32_set_service_status(int status, [int checkpoint])
   Update the service status */
static PHP_FUNCTION(win32_set_service_status)
{
	if (strcmp(sapi_module.name, "cli") != 0) {
		zend_error(E_ERROR, "This function work only when using the CLI SAPI and called into the service code.");
		RETURN_FALSE;
	}

	long status;
	long checkpoint = 0;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &status, &checkpoint)) {
		RETURN_FALSE;
	}

	SVCG(st.dwCurrentState) = status;
	/* CheckPoints are only valid for the SERVICE_*_PENDING statuses. */
	if ((status == SERVICE_CONTINUE_PENDING) || (status == SERVICE_PAUSE_PENDING) || (status == SERVICE_START_PENDING) || (status == SERVICE_STOP_PENDING)) {
		SVCG(st.dwCheckPoint) = checkpoint;
	}

	if (!SetServiceStatus(SVCG(sh), &SVCG(st))) {
		RETURN_LONG(GetLastError())
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto long win32_create_service(array details [, string machine])
   Creates a new service entry in the SCM database */
static PHP_FUNCTION(win32_create_service)
{
	zval *tmp;
	zval *details;
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
	char *load_order;
	char **deps = NULL;
	char *desc;
	BOOL delayed_start;
	SC_HANDLE hsvc, hmgr;
	char *path_and_params;
	SERVICE_DESCRIPTION srvc_desc;
	SERVICE_DELAYED_AUTO_START_INFO srvc_delayed_start;
	OSVERSIONINFO osvi;
	DWORD base_priority;
	HKEY hKey;
	char *service_key;
	long registry_result;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|s!", &details, &machine, &machine_len)) {
		RETURN_FALSE;
	}

#define STR_DETAIL(name, var, def) \
	if ((tmp = zend_hash_str_find(Z_ARRVAL_P(details), name, sizeof(name)-1)) != NULL) { \
		if (IS_NULL != Z_TYPE_P(tmp)) { \
			convert_to_string_ex(tmp); \
		} else { \
			convert_to_null_ex(tmp); \
		} \
		if (strlen(Z_STRVAL_P(tmp)) != Z_STRLEN_P(tmp)) { \
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "malformed " name); \
			RETURN_FALSE; \
		} \
		var = Z_STRVAL_P(tmp); \
	} else { \
		var = def; \
	}

#define INT_DETAIL(name, var, def) \
	if ((tmp = zend_hash_find(Z_ARRVAL_P(details), zend_string_init(name, sizeof(name), 0))) != NULL) { \
		convert_to_long_ex(tmp); \
		var = Z_LVAL_P(tmp); \
	} else { \
		var = def; \
	}

#define BOOL_DETAIL(name, var, def) \
	if ((tmp = zend_hash_find(Z_ARRVAL_P(details), zend_string_init(name, sizeof(name), 0))) != NULL) { \
		convert_to_boolean_ex(tmp); \
		var = Z_LVAL_P(tmp); \
	} else { \
		var = def; \
	}

	STR_DETAIL(INFO_SERVICE, service, NULL);
	STR_DETAIL(INFO_DISPLAY, display, NULL);
	STR_DETAIL(INFO_USER, user, NULL);
	STR_DETAIL(INFO_PASSWORD, password, "");
	STR_DETAIL(INFO_PATH, path, NULL);
	STR_DETAIL(INFO_PARAMS, params, "");
	STR_DETAIL("load_order", load_order, NULL);
	STR_DETAIL(INFO_DESCRIPTION, desc, NULL);
	INT_DETAIL("svc_type", svc_type, SERVICE_WIN32_OWN_PROCESS);
	INT_DETAIL(INFO_START_TYPE, start_type, SERVICE_AUTO_START);
	INT_DETAIL("error_control", error_control, SERVICE_ERROR_IGNORE);
	BOOL_DETAIL("delayed_start", delayed_start, 0); /* Allow Vista+ delayed service start. */
	INT_DETAIL("base_priority", base_priority, NORMAL_PRIORITY_CLASS);

	if (service == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "missing vital parameters");
		RETURN_FALSE;
	}

	srvc_desc.lpDescription = desc;
	srvc_delayed_start.fDelayedAutostart = delayed_start;

	/* Connect to the SCManager. */
	hmgr = OpenSCManager(machine, NULL, SC_MANAGER_ALL_ACCESS);

	/* Quit if no connection. */
	if (!hmgr) {
		RETURN_LONG(GetLastError());
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
			(LPCSTR)deps,
			(LPCSTR)user,
			(LPCSTR)password);

	efree(path_and_params);

	/* Get the current OS version. */
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	/* If there was an error :
	   - Creating the service
	   - Setting the service description
	   - Setting the delayed start - only on Windows Vista and greater and if the service start type is auto start.
	   then track the error. */
	if (	!hsvc ||
		!ChangeServiceConfig2(hsvc, SERVICE_CONFIG_DESCRIPTION, &srvc_desc) ||
		(start_type & SERVICE_AUTO_START && osvi.dwMajorVersion >= 6 && !ChangeServiceConfig2(hsvc, SERVICE_CONFIG_DELAYED_AUTO_START_INFO, &srvc_delayed_start))
		) {
		RETVAL_LONG(GetLastError());
	} else {
		RETVAL_LONG(NO_ERROR);
	}

	CloseServiceHandle(hsvc);
	CloseServiceHandle(hmgr);

	/* Store the base_priority in the registry. */
	spprintf(&service_key, 0, "%s%s", SERVICES_REG_KEY_ROOT, service);
	if (ERROR_SUCCESS != (registry_result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, service_key, 0, KEY_ALL_ACCESS, &hKey))) {
		RETVAL_LONG(registry_result);
	} else if (ERROR_SUCCESS != (registry_result = RegSetValueEx(hKey, SERVICES_REG_BASE_PRIORITY, 0, REG_DWORD, (CONST BYTE*)&base_priority, sizeof(REG_DWORD)))) {
		RETVAL_LONG(registry_result);
	} else {
		RegCloseKey(hKey);
	}
	efree(service_key);

}
/* }}} */

/* {{{ proto long win32_delete_service(string servicename [, string machine])
   Deletes a service entry from the SCM database */
static PHP_FUNCTION(win32_delete_service)
{
	char *machine = NULL;
	char *service = NULL;
	size_t machine_len = 0;
	size_t	service_len = 0;
	SC_HANDLE hsvc;
	SC_HANDLE hmgr;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s!", &service, &service_len, &machine, &machine_len)) {
		RETURN_FALSE;
	}

	hmgr = OpenSCManager(machine, NULL, SC_MANAGER_ALL_ACCESS);
	if (hmgr) {
		hsvc = OpenService(hmgr, service, DELETE);
		if (hsvc) {
			if (DeleteService(hsvc)) {
				RETVAL_LONG(NO_ERROR);
			} else {
				RETVAL_LONG(GetLastError());
			}
			CloseServiceHandle(hsvc);
		} else {
			RETVAL_LONG(GetLastError());
		}
		CloseServiceHandle(hmgr);
	} else {
		RETVAL_LONG(GetLastError());
	}
}
/* }}} */

/* {{{ proto long win32_get_last_control_message()
   Returns the last control message that was sent to this service process */
static PHP_FUNCTION(win32_get_last_control_message)
{
	if (strcmp(sapi_module.name, "cli") != 0) {
		zend_error(E_ERROR, "This function work only when using the CLI SAPI and called into the service code.");
		RETURN_FALSE;
	}

	RETURN_LONG(SVCG(args.dwControl));
}
/* }}} */

/* {{{ proto mixed win32_query_service_status(string servicename [, string machine])
   Queries the status of a service */
static PHP_FUNCTION(win32_query_service_status)
{
	char *machine = NULL;
	char *service = NULL;
	size_t machine_len = 0;
	size_t	service_len = 0;
	SC_HANDLE hsvc;
	SC_HANDLE hmgr;
	SERVICE_STATUS_PROCESS *st;
	DWORD size;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s!", &service, &service_len, &machine, &machine_len)) {
		RETURN_FALSE;
	}

	hmgr = OpenSCManager(machine, NULL, GENERIC_READ);
	if (hmgr) {
		hsvc = OpenService(hmgr, service, SERVICE_QUERY_STATUS);
		if (hsvc) {
			size = sizeof(*st);
			st = emalloc(size);
			if (!QueryServiceStatusEx(hsvc, SC_STATUS_PROCESS_INFO,
					(LPBYTE)st, size, &size)) {
				if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
					RETVAL_LONG(GetLastError());
					goto out_fail;
				}
				st = erealloc(st, size);
				if (!QueryServiceStatusEx(hsvc, SC_STATUS_PROCESS_INFO,
						(LPBYTE)st, size, &size)) {
					RETVAL_LONG(GetLastError());
					goto out_fail;
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
out_fail:
			efree(st);
			CloseServiceHandle(hsvc);
		} else {
			RETVAL_LONG(GetLastError());
		}
		CloseServiceHandle(hmgr);
	} else {
		RETVAL_LONG(GetLastError());
	}
}
/* }}} */

/* {{{ proto long win32_start_service(string servicename [, string machine])
   Starts a service */
static PHP_FUNCTION(win32_start_service)
{
	char *machine = NULL;
	char *service = NULL;
	size_t machine_len = 0;
	size_t	service_len = 0;
	SC_HANDLE hsvc;
	SC_HANDLE hmgr;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s!", &service, &service_len, &machine, &machine_len)) {
		RETURN_FALSE;
	}

	hmgr = OpenSCManager(machine, NULL, SC_MANAGER_CONNECT);
	if (hmgr) {
		hsvc = OpenService(hmgr, service, SERVICE_START);
		if (hsvc) {
			if (StartService(hsvc, 0, NULL)) {
				RETVAL_LONG(NO_ERROR);
			} else {
				RETVAL_LONG(GetLastError());
			}
			CloseServiceHandle(hsvc);
		} else {
			RETVAL_LONG(GetLastError());
		}
		CloseServiceHandle(hmgr);
	} else {
		RETVAL_LONG(GetLastError());
	}
}
/* }}} */

static void win32_handle_service_controls(INTERNAL_FUNCTION_PARAMETERS, long access, long status) /* {{{ */
{
	char *machine = NULL;
	char *service = NULL;
	size_t machine_len = 0;
	size_t	service_len = 0;
	SC_HANDLE hsvc;
	SC_HANDLE hmgr;
	SERVICE_STATUS st;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s!", &service, &service_len, &machine, &machine_len)) {
		RETURN_FALSE;
	}

	hmgr = OpenSCManager(machine, NULL, SC_MANAGER_CONNECT);
	if (hmgr) {
		hsvc = OpenService(hmgr, service, access);
		if (hsvc) {
			if (ControlService(hsvc, status, &st)) {
				RETVAL_LONG(NO_ERROR);
			} else {
				RETVAL_LONG(GetLastError());
			}
			CloseServiceHandle(hsvc);
		} else {
			RETVAL_LONG(GetLastError());
		}
		CloseServiceHandle(hmgr);
	} else {
		RETVAL_LONG(GetLastError());
	}
}

/* {{{ proto long win32_stop_service(string servicename [, string machine])
   Stops a service */
static PHP_FUNCTION(win32_stop_service)
{
	win32_handle_service_controls(INTERNAL_FUNCTION_PARAM_PASSTHRU, SERVICE_STOP, SERVICE_CONTROL_STOP);
}
/* }}} */

/* {{{ proto long win32_pause_service(string servicename [, string machine])
   Pauses a service */
static PHP_FUNCTION(win32_pause_service)
{
	win32_handle_service_controls(INTERNAL_FUNCTION_PARAM_PASSTHRU, SERVICE_PAUSE_CONTINUE, SERVICE_CONTROL_PAUSE);
}
/* }}} */

/* {{{ proto long win32_continue_service(string servicename [, string machine])
   Resumes a service */
static PHP_FUNCTION(win32_continue_service)
{
	win32_handle_service_controls(INTERNAL_FUNCTION_PARAM_PASSTHRU, SERVICE_PAUSE_CONTINUE, SERVICE_CONTROL_CONTINUE);
}
/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_win32_start_service_ctrl_dispatcher, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_win32_set_service_status, 0, 0, 1)
	ZEND_ARG_INFO(0, status)
	ZEND_ARG_INFO(0, checkpoint)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_win32_create_service, 0, 0, 1)
	ZEND_ARG_INFO(0, details)
	ZEND_ARG_INFO(0, machine)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_win32_delete_service, 0, 0, 1)
	ZEND_ARG_INFO(0, servicename)
	ZEND_ARG_INFO(0, machine)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_win32_get_last_control_message, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_win32_query_service_status, 0, 0, 1)
	ZEND_ARG_INFO(0, servicename)
	ZEND_ARG_INFO(0, machine)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_win32_start_service, 0, 0, 1)
	ZEND_ARG_INFO(0, servicename)
	ZEND_ARG_INFO(0, machine)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_win32_stop_service, 0, 0, 1)
	ZEND_ARG_INFO(0, servicename)
	ZEND_ARG_INFO(0, machine)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_win32_pause_service, 0, 0, 1)
	ZEND_ARG_INFO(0, servicename)
	ZEND_ARG_INFO(0, machine)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_win32_continue_service, 0, 0, 1)
	ZEND_ARG_INFO(0, servicename)
	ZEND_ARG_INFO(0, machine)
ZEND_END_ARG_INFO()
/* }}} */

static zend_function_entry functions[] = {
	PHP_FE(win32_start_service_ctrl_dispatcher, arginfo_win32_start_service_ctrl_dispatcher)
	PHP_FE(win32_set_service_status,            arginfo_win32_set_service_status)
	PHP_FE(win32_create_service,                arginfo_win32_create_service)
	PHP_FE(win32_delete_service,                arginfo_win32_delete_service)
	PHP_FE(win32_get_last_control_message,      arginfo_win32_get_last_control_message)
	PHP_FE(win32_query_service_status,          arginfo_win32_query_service_status)
	PHP_FE(win32_start_service,                 arginfo_win32_start_service)
	PHP_FE(win32_stop_service,                  arginfo_win32_stop_service)
	PHP_FE(win32_pause_service,                 arginfo_win32_pause_service)
	PHP_FE(win32_continue_service,              arginfo_win32_continue_service)
	PHP_FE_END
};

static void init_globals(zend_win32service_globals *g)
{
	memset(g, 0, sizeof(*g));
}
/*
 * Return 0 if this ext is not compatible with the current php version, 1 otherwise.
 * This function work only if the runtime is ready !
 */
static int check_php_version()
{
	zval result;
	zval function_name;
	char *result2 = NULL;

	ZVAL_STRING(&function_name, "phpversion");

	if (call_user_function(EG(function_table), NULL, &function_name, &result, 0, NULL) == SUCCESS) {
		result2 = Z_STRVAL_P(&result);
		//php_printf("VERSION = %s\n", result2);
		zval_ptr_dtor(&function_name);
		zval_ptr_dtor(&result);

		//php_printf("M=%d m=%d r=%d\n", PHP_MAJOR_VERSION, PHP_MINOR_VERSION, PHP_RELEASE_VERSION);

		if (php_version_compare(result2, "7.0.0") == 0) {
			//zend_error(E_CORE_ERROR, "The Win32Service extension not work on PHP 7.0.0. Work with 7.0.1+");
			return 0;
		}
		if (php_version_compare(result2, "7.1.0") == 0) {
			//zend_error(E_CORE_ERROR, "The Win32Service extension not work on PHP 7.1.0. Work with 7.1.1+");
			return 0;
		}
		efree(result2);
	}/* else {
		php_printf("call_user_function fail\n");
	}*/
	return 1;
}

static PHP_MINIT_FUNCTION(win32service)
{
	/*if (strcmp(sapi_module.name, "cli") != 0) {
		zend_error(E_NOTICE, "The Win32Service extension does work when using the CLI SAPI with administrator right level. On other SAPI, please check security consideration.");
	}*/

	/*if (check_php_version() == 0) {
		return FAILURE;
	}*/

	

	ZEND_INIT_MODULE_GLOBALS(win32service, init_globals, NULL);

#define MKCONST(x)	REGISTER_LONG_CONSTANT("WIN32_" # x, x, CONST_CS|CONST_PERSISTENT)
#define MKSCONST(x)	REGISTER_STRING_CONSTANT("WIN32_" # x, x, CONST_CS|CONST_PERSISTENT)

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
	MKCONST(NO_ERROR);                                 /* 0x00000000 No error. */

	/* Win32 Priority Constants */
	MKCONST(ABOVE_NORMAL_PRIORITY_CLASS);              /* 0x00008000 Process that has priority above NORMAL_PRIORITY_CLASS but below HIGH_PRIORITY_CLASS. */
	MKCONST(BELOW_NORMAL_PRIORITY_CLASS);              /* 0x00004000 Process that has priority above IDLE_PRIORITY_CLASS but below NORMAL_PRIORITY_CLASS. */
	MKCONST(HIGH_PRIORITY_CLASS);                      /* 0x00000080 Process that performs time-critical tasks that must be executed immediately. The threads of the process preempt the threads of normal or idle priority class processes. An example is the Task List, which must respond quickly when called by the user, regardless of the load on the operating system. Use extreme care when using the high-priority class, because a high-priority class application can use nearly all available CPU time. */
	MKCONST(IDLE_PRIORITY_CLASS);                      /* 0x00000040 Process whose threads run only when the system is idle. The threads of the process are preempted by the threads of any process running in a higher priority class. An example is a screen saver. The idle-priority class is inherited by child processes. */
	MKCONST(NORMAL_PRIORITY_CLASS);                    /* 0x00000020 Process with no special scheduling needs. */
	MKCONST(REALTIME_PRIORITY_CLASS);                  /* 0x00000100 Process that has the highest possible priority. The threads of the process preempt the threads of all other processes, including operating system processes performing important tasks. For example, a real-time process that executes for more than a very brief interval can cause disk caches not to flush or cause the mouse to be unresponsive. */

    /* Win32 Service informations */
    MKSCONST(INFO_SERVICE);
    MKSCONST(INFO_DISPLAY);
    MKSCONST(INFO_USER);
    MKSCONST(INFO_PASSWORD);
    MKSCONST(INFO_PATH);
    MKSCONST(INFO_PARAMS);
    MKSCONST(INFO_DESCRIPTION);
    MKSCONST(INFO_START_TYPE);

	return SUCCESS;
}

static PHP_RSHUTDOWN_FUNCTION(win32service)
{
	if (SVCG(sh)) {
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

static PHP_MINFO_FUNCTION(win32service)
{
	php_info_print_table_start();
	if (check_php_version() == 0) {
		php_info_print_table_row(2, "Caution", "The Win32Service extension not work on PHP 7.0.0 or PHP 7.1.0. Work with 7.0.1+ or 7.1.1+");
	}
	
	php_info_print_table_row(2, "Win32 Service support", "enabled");
	php_info_print_table_row(2, "Version", PHP_WIN32SERVICE_VERSION);
	php_info_print_table_row(2, "Current SAPI", sapi_module.name);
	if (strcmp(sapi_module.name, "cli") != 0) {
		php_info_print_table_row(2, "NOTE", "The Win32Service extension does work when using the CLI SAPI with administrator right level. On other SAPI, please check security consideration.");
	}
	php_info_print_table_end();

	php_info_print_table_start();
	php_info_print_table_header(2, "Function", "State for the current SAPI");
	if (!strcmp(sapi_module.name, "cli")) {
		php_info_print_table_row(2, "win32_start_service_ctrl_dispatcher", "enabled");
		php_info_print_table_row(2, "win32_set_service_status", "enabled");
		php_info_print_table_row(2, "win32_get_last_control_message", "enabled");
	} else {
		php_info_print_table_row(2, "win32_start_service_ctrl_dispatcher", "disabled");
		php_info_print_table_row(2, "win32_set_service_status", "disabled");
		php_info_print_table_row(2, "win32_get_last_control_message", "disabled");
	}
	php_info_print_table_row(2, "win32_create_service", "enabled");
	php_info_print_table_row(2, "win32_delete_service", "enabled");
	php_info_print_table_row(2, "win32_query_service_status", "enabled");
	php_info_print_table_row(2, "win32_start_service", "enabled");
	php_info_print_table_row(2, "win32_stop_service", "enabled");
	php_info_print_table_row(2, "win32_pause_service", "enabled");
	php_info_print_table_row(2, "win32_continue_service", "enabled");
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


