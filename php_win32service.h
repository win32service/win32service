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
extern PHPAPI zend_class_entry * win32service_Win32ServiceException;

zend_class_entry *Win32ServiceException_ce_ptr;

#define phpext_win32service_ptr    &win32service_module_entry

#define PHP_WIN32SERVICE_VERSION "1.1.0-dev"

#ifndef PHP_WIN32
# error This extension is for win32 only
#endif

#if PHP_MAJOR_VERSION < 8
# error This extension is for PHP 8+ only
#endif

typedef struct _win32service_right_info_object win32service_right_info_object;

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
#define INFO_LOAD_ORDER        "load_order"
#define INFO_SVC_TYPE        "svc_type"
#define INFO_ERROR_CONTROL    "error_control"
#define INFO_DELAYED_START    "delayed_start"
#define INFO_BASE_PRIORITY    "base_priority"
#define INFO_DEPENDENCIES   "dependencies"
#define INFO_RECOVERY_DELAY "recovery_delay"
#define INFO_RECOVERY_ACTION_1 "recovery_action_1"
#define INFO_RECOVERY_ACTION_2 "recovery_action_2"
#define INFO_RECOVERY_ACTION_3 "recovery_action_3"
#define INFO_RECOVERY_RESET_PERIOD "recovery_reset_period"
#define INFO_RECOVERY_ENABLED "recovery_enabled"
#define INFO_RECOVERY_REBOOT_MSG "recovery_reboot_msg"
#define INFO_RECOVERY_COMMAND "recovery_command"

/* NOTE: due to how internally PHP works with custom object, zend_object position in structure matters */
struct _win32service_right_info_object {
    long mask;
    long accessMode;
    char * domain;
    char * username;
    zend_object zo;
};

static inline win32service_right_info_object *php_win32service_right_info_object_fetch(zend_object *obj)
{
    return (win32service_right_info_object *) ((char *) obj - XtOffsetOf(win32service_right_info_object, zo));
}

#define PHP_WIN32SERVICE_DECLARE_PROPERTY_TYPE(type, nullable) (zend_type) ZEND_TYPE_INIT_CODE(type, nullable, 0)

#define PHP_WIN32SERVICE_NULLABLE_DEFAULT_INIT(val, nullable)                                                                  \
    zval val;                                                                                                          \
    if (nullable) {                                                                                                    \
        ZVAL_NULL(&val);                                                                                               \
    } else {                                                                                                           \
        ZVAL_UNDEF(&val);                                                                                              \
    }
#define PHP_WIN32SERVICE_DECLARE_TYPED_PROPERTY_ZVAL(class_entry, name, flags, type_info, val)                                 \
    {                                                                                                                  \
        zend_string *__name = zend_string_init(ZEND_STRL(name), 1);                                                    \
        zend_declare_typed_property(class_entry, __name, &(val), flags, NULL, type_info);                              \
        zend_string_release(__name);                                                                                   \
    }
#define PHP_WIN32SERVICE_DECLARE_TYPED_PROPERTY(class_entry, name, flags, type, nullable)                                      \
    {                                                                                                                  \
        PHP_WIN32SERVICE_NULLABLE_DEFAULT_INIT(__val, nullable);                                                               \
        PHP_WIN32SERVICE_DECLARE_TYPED_PROPERTY_ZVAL(                                                                          \
            class_entry,                                                                                               \
            name,                                                                                                      \
            flags,                                                                                                     \
            PHP_WIN32SERVICE_DECLARE_PROPERTY_TYPE(type, nullable),                                                            \
            __val                                                                                                      \
        )                                                                                                              \
    }
#define PHP_WIN32SERVICE_DECLARE_TYPED_PROPERTY_WITH_DEFAULT(class_entry, name, flags, type, nullable, init)                   \
    {                                                                                                                  \
        zval __val;                                                                                                    \
        ZVAL_UNDEF(&__val);                                                                                            \
        init(&__val);                                                                                                  \
        PHP_WIN32SERVICE_DECLARE_TYPED_PROPERTY_ZVAL(                                                                          \
            class_entry,                                                                                               \
            name,                                                                                                      \
            flags,                                                                                                     \
            PHP_WIN32SERVICE_DECLARE_PROPERTY_TYPE(type, nullable),                                                            \
            __val                                                                                                      \
        )                                                                                                              \
    }


#endif


