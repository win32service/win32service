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

#ifndef WIN32SERVICE_CONFIG_H
#define WIN32SERVICE_CONFIG_H

#include <windows.h>
#include "php.h"

char *win32_generate_path_and_params(char *path, char *params, long svc_type, char *user);
DWORD win32_configure_service_ex(SC_HANDLE hsvc, zval *details, BOOL is_update, DWORD start_type, char **error_msg);

#endif /* WIN32SERVICE_CONFIG_H */
