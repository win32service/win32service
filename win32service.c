/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2005 The PHP Group                                |
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
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_win32service.h"
#include "php_win32service_int.h"

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
	MSG msg;
	zend_win32service_globals *g = (zend_win32service_globals*)tmp_service_g;

	g->st.dwServiceType = SERVICE_WIN32;
	g->st.dwCurrentState = SERVICE_START_PENDING;
	g->st.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	
	g->sh = RegisterServiceCtrlHandlerEx(g->service_name, service_handler, g);

	if (g->sh == (SERVICE_STATUS_HANDLE)0) {
		g->code = GetLastError();
		SetEvent(g->event);
		return;
	}

	g->st.dwCurrentState = SERVICE_RUNNING;

	if (!SetServiceStatus(g->sh, &g->st)) {
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
	char *name;
	int name_len;
	
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

/* {{{ proto bool win32_set_service_status(int status)
   Update the service status */
static PHP_FUNCTION(win32_set_service_status)
{
	long status;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &status)) {
		RETURN_FALSE;
	}

	SVCG(st.dwCurrentState) = status;
	SetServiceStatus(SVCG(sh), &SVCG(st));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto long win32_create_service(array details [, string machine])
   Creates a new service entry in the SCM database */
static PHP_FUNCTION(win32_create_service)
{
	zval **tmp;
	zval *details;
	char *machine = NULL;
	int machine_len;
	char *service;
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
	SC_HANDLE hsvc, hmgr;
	char *path_and_params;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|s!", &details, &machine, &machine_len)) {
		RETURN_FALSE;
	}

#define STR_DETAIL(name, var, def)	\
	if (SUCCESS == zend_hash_find(Z_ARRVAL_P(details), name, sizeof(name), (void**)&tmp)) { \
		convert_to_string_ex(tmp); \
		var = Z_STRVAL_PP(tmp); \
	} else { \
		var = def; \
	}

#define INT_DETAIL(name, var, def) \
	if (SUCCESS == zend_hash_find(Z_ARRVAL_P(details), name, sizeof(name), (void**)&tmp)) { \
		convert_to_long_ex(tmp); \
		var = Z_LVAL_PP(tmp); \
	} else { \
		var = def; \
	}

	STR_DETAIL("service", service, NULL);
	STR_DETAIL("display", display, NULL);
	STR_DETAIL("user", user, NULL);
	STR_DETAIL("password", password, "");
	STR_DETAIL("path", path, NULL);	
	STR_DETAIL("params", params, "");
	STR_DETAIL("load_order", load_order, NULL);
	STR_DETAIL("description", desc, NULL);
	INT_DETAIL("svc_type", svc_type, SERVICE_WIN32_OWN_PROCESS);
	INT_DETAIL("start_type", start_type, SERVICE_AUTO_START);
	INT_DETAIL("error_control", error_control, SERVICE_ERROR_IGNORE);

	if (service == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "missing vital parameters");
		RETURN_FALSE;
	}

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

	hmgr = OpenSCManager(machine, NULL, SC_MANAGER_ALL_ACCESS);
	if (hmgr) {
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
				deps,
				user,
				password);

		if (hsvc) {
			CloseServiceHandle(hsvc);
			RETVAL_TRUE;
		} else {
			RETVAL_LONG(GetLastError());
		}
		CloseServiceHandle(hmgr);
	} else {
		RETVAL_LONG(GetLastError());
	}

	efree(path_and_params);
}
/* }}} */

/* {{{ proto long win32_delete_service(string servicename [, string machine])
   Deletes a service entry from the SCM database */
static PHP_FUNCTION(win32_delete_service)
{
	char *machine = NULL, *service;
	int machine_len, service_len;
	SC_HANDLE hsvc, hmgr;

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

static PHP_FUNCTION(win32_get_last_control_message)
{
	RETURN_LONG(SVCG(args.dwControl));
}

static function_entry functions[] = {
	PHP_FE(win32_start_service_ctrl_dispatcher, NULL)
	PHP_FE(win32_set_service_status, NULL)
	PHP_FE(win32_create_service, NULL)
	PHP_FE(win32_delete_service, NULL)
	PHP_FE(win32_get_last_control_message, NULL)
	{NULL, NULL, NULL}
};

static void init_globals(zend_win32service_globals *g)
{
	memset(g, 0, sizeof(*g));
}

static PHP_MINIT_FUNCTION(win32service)
{
	ZEND_INIT_MODULE_GLOBALS(win32service, init_globals, NULL);

	REGISTER_LONG_CONSTANT("WIN32_SERVICE_CONTROL_CONTINUE", SERVICE_CONTROL_CONTINUE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("WIN32_SERVICE_CONTROL_INTERROGATE", SERVICE_CONTROL_INTERROGATE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("WIN32_SERVICE_CONTROL_PAUSE", SERVICE_CONTROL_PAUSE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("WIN32_SERVICE_CONTROL_STOP", SERVICE_CONTROL_STOP, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("WIN32_SERVICE_CONTROL_HARDWAREPROFILECHANGE", SERVICE_CONTROL_HARDWAREPROFILECHANGE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("WIN32_SERVICE_CONTROL_POWEREVENT", SERVICE_CONTROL_POWEREVENT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("WIN32_SERVICE_CONTROL_SESSIONCHANGE", SERVICE_CONTROL_SESSIONCHANGE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("WIN32_ERROR_CALL_NOT_IMPLEMENTED", ERROR_CALL_NOT_IMPLEMENTED, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("WIN32_NO_ERROR", NO_ERROR, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("WIN32_SERVICE_RUNNING", SERVICE_RUNNING, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("WIN32_SERVICE_STOPPED", SERVICE_STOPPED, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("WIN32_SERVICE_STOP_PENDING", SERVICE_STOP_PENDING, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("WIN32_SERVICE_OWN_PROCESS", SERVICE_WIN32_OWN_PROCESS, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("WIN32_SERVICE_INTERACTIVE_PROCESS", SERVICE_INTERACTIVE_PROCESS, CONST_CS|CONST_PERSISTENT);

	return SUCCESS;
}

static PHP_RSHUTDOWN_FUNCTION(win32service)
{
	if (SVCG(sh)) {
		SVCG(st).dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(SVCG(sh), &SVCG(st));
//		PostThreadMessage(SVCG(svc_thread_id), WM_QUIT, 0, 0);
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
	php_info_print_table_header(2, "Win32 Service support", "enabled");
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
	"0.1",
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_WIN32SERVICE
ZEND_GET_MODULE(win32service)
#endif


