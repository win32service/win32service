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
#include "zend_exceptions.h"
#include "php_win32service.h"
#include "win32service_error.h"

void convert_error_to_exception(DWORD code, const char *message) {
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
