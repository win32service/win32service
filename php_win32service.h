/*
  +----------------------------------------------------------------------+
  | PHP Version 8                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2020 The PHP Group                                |
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

/* $Id: php_win32service.h 309260 2011-03-15 17:03:59Z rquadling $ */

#ifndef PHP_WIN32SERVICE_H
#define PHP_WIN32SERVICE_H

extern zend_module_entry win32service_module_entry;
#define phpext_win32service_ptr	&win32service_module_entry

#define PHP_WIN32SERVICE_VERSION "1.0.0-alpha1"

#ifndef PHP_WIN32
# error This extension is for win32 only
#endif

#if PHP_MAJOR_VERSION < 8
# error This extension is for PHP 8+ only
#endif

#ifndef SERVICE_WIN32_OWN_PROCESS_INTERACTIVE
#define SERVICE_WIN32_OWN_PROCESS_INTERACTIVE SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS
#endif

#define INFO_SERVICE        "service"
#define INFO_DISPLAY        "display"
#define INFO_USER           "user"
#define INFO_PASSWORD       "password"
#define INFO_PATH           "path"
#define INFO_PARAMS         "params"
#define INFO_DESCRIPTION    "description"
#define INFO_START_TYPE     "start_type"
#define INFO_LOAD_ORDER		"load_order"
#define INFO_SVC_TYPE		"svc_type"
#define INFO_ERROR_CONTROL	"error_control"
#define INFO_DELAYED_START	"delayed_start"
#define INFO_BASE_PRIORITY	"base_priority"
#define INFO_DEPENDENCIES   "dependencies"
#define INFO_RECOVERY_DELAY "recovery_delay"
#define INFO_RECOVERY_ACTION_1 "recovery_action_1"
#define INFO_RECOVERY_ACTION_2 "recovery_action_2"
#define INFO_RECOVERY_ACTION_3 "recovery_action_3"
#define INFO_RECOVERY_RESET_PERIOD "recovery_reset_period"
#define INFO_RECOVERY_ENABLED "recovery_enabled"
#define INFO_RECOVERY_REBOOT_MSG "recovery_reboot_msg"
#define INFO_RECOVERY_COMMAND "recovery_command"


#endif


