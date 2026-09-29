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
#include "win32service_wide.h"

wchar_t *win32_internal_to_wide_n(const char *src, int cb, int *out_cch) {
    int needed;
    wchar_t *out;

    if (src == NULL) {
        return NULL;
    }
    if (cb == 0) {
        out = (wchar_t *) emalloc(sizeof(wchar_t));
        out[0] = L'\0';
        if (out_cch) {
            *out_cch = 0;
        }
        return out;
    }

    needed = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, src, cb, NULL, 0);
    if (needed <= 0) {
        SetLastError(ERROR_NO_UNICODE_TRANSLATION);
        return NULL;
    }
    out = (wchar_t *) safe_emalloc((size_t) needed + 1, sizeof(wchar_t), 0);
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, src, cb, out, needed) <= 0) {
        efree(out);
        SetLastError(ERROR_NO_UNICODE_TRANSLATION);
        return NULL;
    }
    out[needed] = L'\0';
    if (out_cch) {
        *out_cch = needed;
    }
    return out;
}

BOOL win32_internal_to_wide(const char *src, wchar_t **out) {
    *out = NULL;
    if (src == NULL) {
        return TRUE;
    }
    *out = win32_internal_to_wide_n(src, -1, NULL);
    return *out != NULL;
}

char *win32_internal_to_utf8_n(const wchar_t *src, int cch, int *out_len) {
    int needed;
    char *out;

    if (src == NULL) {
        return NULL;
    }
    if (cch == 0) {
        out = (char *) emalloc(1);
        out[0] = '\0';
        if (out_len) {
            *out_len = 0;
        }
        return out;
    }

    needed = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, src, cch, NULL, 0, NULL, NULL);
    if (needed <= 0) {
        SetLastError(ERROR_NO_UNICODE_TRANSLATION);
        return NULL;
    }
    out = (char *) safe_emalloc((size_t) needed + 1, sizeof(char), 0);
    if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, src, cch, out, needed, NULL, NULL) <= 0) {
        efree(out);
        SetLastError(ERROR_NO_UNICODE_TRANSLATION);
        return NULL;
    }
    out[needed] = '\0';
    if (out_len) {
        *out_len = needed;
    }
    return out;
}

char *win32_internal_to_utf8(const wchar_t *src) {
    return win32_internal_to_utf8_n(src, -1, NULL);
}

void win32_internal_free_secret(wchar_t *str) {
    if (str == NULL) {
        return;
    }
    SecureZeroMemory(str, wcslen(str) * sizeof(wchar_t));
    efree(str);
}

char *win32_internal_getenv_utf8(const wchar_t *name) {
    DWORD len = GetEnvironmentVariableW(name, NULL, 0);
    wchar_t *value;
    char *result;

    if (len == 0) {
        return NULL;
    }
    value = (wchar_t *) safe_emalloc(len, sizeof(wchar_t), 0);
    if (GetEnvironmentVariableW(name, value, len) == 0) {
        efree(value);
        return NULL;
    }
    result = win32_internal_to_utf8(value);
    efree(value);
    return result;
}

/* The wrappers below convert the arguments, call the wide function and release the
   temporary buffers. The last error of the Windows call is preserved. */

SC_HANDLE win32_internal_open_sc_manager(const char *machine, DWORD access) {
    wchar_t *machine_w = NULL;
    SC_HANDLE handle;
    DWORD error;

    if (!win32_internal_to_wide(machine, &machine_w)) {
        return NULL;
    }
    handle = OpenSCManagerW(machine_w, NULL, access);
    error = GetLastError();
    if (machine_w) {
        efree(machine_w);
    }
    SetLastError(error);
    return handle;
}

SC_HANDLE win32_internal_open_service(SC_HANDLE hmgr, const char *service, DWORD access) {
    wchar_t *service_w = NULL;
    SC_HANDLE handle;
    DWORD error;

    if (!win32_internal_to_wide(service, &service_w)) {
        return NULL;
    }
    handle = OpenServiceW(hmgr, service_w, access);
    error = GetLastError();
    if (service_w) {
        efree(service_w);
    }
    SetLastError(error);
    return handle;
}

SC_HANDLE win32_internal_create_service(SC_HANDLE hmgr, const char *service, const char *display,
                                        DWORD svc_type, DWORD start_type, DWORD error_control,
                                        const char *path, const char *load_order, const wchar_t *deps,
                                        const char *user, const char *password) {
    wchar_t *service_w = NULL, *display_w = NULL, *path_w = NULL, *load_order_w = NULL, *user_w = NULL, *password_w = NULL;
    SC_HANDLE handle = NULL;
    DWORD error;

    if (!win32_internal_to_wide(service, &service_w) ||
        !win32_internal_to_wide(display, &display_w) ||
        !win32_internal_to_wide(path, &path_w) ||
        !win32_internal_to_wide(load_order, &load_order_w) ||
        !win32_internal_to_wide(user, &user_w) ||
        !win32_internal_to_wide(password, &password_w)) {
        error = GetLastError();
        goto cleanup;
    }

    handle = CreateServiceW(hmgr, service_w, display_w ? display_w : service_w, SERVICE_ALL_ACCESS,
                            svc_type, start_type, error_control, path_w, load_order_w, NULL,
                            deps, user_w, password_w);
    error = GetLastError();

cleanup:
    if (service_w) efree(service_w);
    if (display_w) efree(display_w);
    if (path_w) efree(path_w);
    if (load_order_w) efree(load_order_w);
    if (user_w) efree(user_w);
    win32_internal_free_secret(password_w);
    SetLastError(error);
    return handle;
}

BOOL win32_internal_change_service_config(SC_HANDLE hsvc, DWORD svc_type, DWORD start_type, DWORD error_control,
                                          const char *path, const char *load_order, const wchar_t *deps,
                                          const char *user, const char *password, const char *display) {
    wchar_t *path_w = NULL, *load_order_w = NULL, *user_w = NULL, *password_w = NULL, *display_w = NULL;
    BOOL result = FALSE;
    DWORD error;

    if (!win32_internal_to_wide(path, &path_w) ||
        !win32_internal_to_wide(load_order, &load_order_w) ||
        !win32_internal_to_wide(user, &user_w) ||
        !win32_internal_to_wide(password, &password_w) ||
        !win32_internal_to_wide(display, &display_w)) {
        error = GetLastError();
        goto cleanup;
    }

    result = ChangeServiceConfigW(hsvc, svc_type, start_type, error_control, path_w, load_order_w, NULL,
                                  deps, user_w, password_w, display_w);
    error = GetLastError();

cleanup:
    if (path_w) efree(path_w);
    if (load_order_w) efree(load_order_w);
    if (user_w) efree(user_w);
    win32_internal_free_secret(password_w);
    if (display_w) efree(display_w);
    SetLastError(error);
    return result;
}
