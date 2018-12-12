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

/* $Id: php_win32service.h 309260 2011-03-15 17:03:59Z rquadling $ */

#ifndef PHP_WIN32SERVICE_H
#define PHP_WIN32SERVICE_H

extern zend_module_entry win32service_module_entry;
#define phpext_win32service_ptr	&win32service_module_entry

#define PHP_WIN32SERVICE_VERSION "0.4.0"

#ifndef PHP_WIN32
# error This extension is for win32 only
#endif

#if PHP_MAJOR_VERSION < 7
# error This extension is for PHP 7+ only
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

#endif


