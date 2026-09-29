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

#ifndef WIN32SERVICE_WIN32SERVICE_WIDE_H
#define WIN32SERVICE_WIN32SERVICE_WIDE_H

#include <windows.h>
#include <winsvc.h>

/*
 * The extension talks to Windows only through the wide (UTF-16) API.
 * PHP strings are handled as UTF-8, and converted at the boundary by the
 * helpers below. All returned buffers are allocated with emalloc().
 */

/* Convert a NUL terminated UTF-8 string. A NULL src gives *out = NULL and TRUE.
   On invalid UTF-8, returns FALSE, *out is NULL and the last error is ERROR_NO_UNICODE_TRANSLATION. */
BOOL win32_internal_to_wide(const char *src, wchar_t **out);

/* Convert cb bytes of UTF-8 (cb = -1 for a NUL terminated string, the terminator is then included).
   *out_cch receives the number of wchar_t written (may be NULL). Returns NULL on failure. */
wchar_t *win32_internal_to_wide_n(const char *src, int cb, int *out_cch);

/* Convert a NUL terminated UTF-16 string. Returns NULL if src is NULL or on failure. */
char *win32_internal_to_utf8(const wchar_t *src);

/* Convert cch wchar_t of UTF-16 (cch = -1 for a NUL terminated string, the terminator is then included).
   *out_len receives the number of bytes written (may be NULL). Returns NULL on failure. */
char *win32_internal_to_utf8_n(const wchar_t *src, int cch, int *out_len);

/* Erase then free a buffer returned by win32_internal_to_wide (used for the passwords). */
void win32_internal_free_secret(wchar_t *str);

/* Value of an environment variable as UTF-8, NULL if it does not exist. */
char *win32_internal_getenv_utf8(const wchar_t *name);

/* Wrappers around the wide Service Control Manager functions, taking UTF-8 strings.
   On failure they return NULL/FALSE and keep the error available through GetLastError(). */
SC_HANDLE win32_internal_open_sc_manager(const char *machine, DWORD access);
SC_HANDLE win32_internal_open_service(SC_HANDLE hmgr, const char *service, DWORD access);
SC_HANDLE win32_internal_create_service(SC_HANDLE hmgr, const char *service, const char *display,
                                        DWORD svc_type, DWORD start_type, DWORD error_control,
                                        const char *path, const char *load_order, const wchar_t *deps,
                                        const char *user, const char *password);
BOOL win32_internal_change_service_config(SC_HANDLE hsvc, DWORD svc_type, DWORD start_type, DWORD error_control,
                                          const char *path, const char *load_order, const wchar_t *deps,
                                          const char *user, const char *password, const char *display);

#endif /* WIN32SERVICE_WIN32SERVICE_WIDE_H */
