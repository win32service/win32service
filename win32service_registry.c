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
  | Author: Jean-Baptiste Nahan <jbnahan@php.net>                    |
  +----------------------------------------------------------------------+
*/
#include "win32service_registry.h"
#include "php.h"
#include <windows.h>
#include <winreg.h>
#include "win32service_wide.h"


/* Open the registry key of a service. The key name is built in UTF-16. */
static LONG win32_internal_open_service_key(char *service, int service_len, HKEY *hKey) {
    wchar_t *service_w = win32_internal_to_wide_n(service, service_len, NULL);
    wchar_t *keyName;
    LONG lReturnValue;

    if (service_w == NULL) {
        return ERROR_NO_UNICODE_TRANSLATION;
    }
    keyName = (wchar_t *) safe_emalloc(wcslen(SERVICES_REG_KEY_ROOT_W) + wcslen(service_w) + 1, sizeof(wchar_t), 0);
    wcscpy(keyName, SERVICES_REG_KEY_ROOT_W);
    wcscat(keyName, service_w);

    lReturnValue = RegOpenKeyExW(HKEY_LOCAL_MACHINE, keyName, 0, KEY_ALL_ACCESS, hKey);
    efree(service_w);
    efree(keyName);

    if (lReturnValue == ERROR_FILE_NOT_FOUND) {
        return ERROR_SERVICE_DOES_NOT_EXIST;
    }
    return lReturnValue;
}

/* Read the Environment value. The registry stores UTF-16, the value is returned as a UTF-8 multi string
   (each string ends with a NUL, and the list with an additional NUL). *pvDataLen is the size in bytes. */
long get_service_environment_vars(char * service, int service_len, char ** pvData, int * pvDataLen) {

    HKEY hKey;
    LONG lReturnValue = 0;
    DWORD dataSize = 0;
    wchar_t *dataW = NULL;

    *pvData = NULL;
    *pvDataLen = 0;

    lReturnValue = win32_internal_open_service_key(service, service_len, &hKey);
    if (lReturnValue != ERROR_SUCCESS) {
        return lReturnValue;
    }

    lReturnValue = RegGetValueW(hKey, NULL, SERVICES_REG_ENVIRONMENT_W, RRF_RT_REG_MULTI_SZ|RRF_ZEROONFAILURE, NULL, NULL, &dataSize);
    if (lReturnValue != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return lReturnValue;
    }
    dataW = (wchar_t *) safe_emalloc(dataSize + sizeof(wchar_t), 1, 0);

    lReturnValue = RegGetValueW(hKey, NULL, SERVICES_REG_ENVIRONMENT_W, RRF_RT_REG_MULTI_SZ|RRF_ZEROONFAILURE, NULL, dataW, &dataSize);
    RegCloseKey(hKey);
    if (lReturnValue != ERROR_SUCCESS) {
        efree(dataW);
        return lReturnValue;
    }

    if (dataSize >= sizeof(wchar_t)) {
        *pvData = win32_internal_to_utf8_n(dataW, (int)(dataSize / sizeof(wchar_t)), pvDataLen);
        if (*pvData == NULL) {
            efree(dataW);
            return ERROR_NO_UNICODE_TRANSLATION;
        }
    } else {
        *pvData = (char *) emalloc(1);
        (*pvData)[0] = '\0';
        *pvDataLen = 0;
    }
    efree(dataW);
    return ERROR_SUCCESS;
}

/* Write the Environment value from a UTF-8 multi string of data_len bytes.
   With data_len = 0 the value is deleted. *step names the failing step. */
long set_service_environment_vars(char * service, int service_len, char * data, int data_len, const char ** step) {
    HKEY hKey;
    LONG lReturnValue;
    int dataW_len = 0;
    wchar_t *dataW = NULL;

    *step = "Error open key";
    lReturnValue = win32_internal_open_service_key(service, service_len, &hKey);
    if (lReturnValue != ERROR_SUCCESS) {
        return lReturnValue;
    }

    if (data_len > 0) {
        *step = "Error create key";
        dataW = win32_internal_to_wide_n(data, data_len, &dataW_len);
        if (dataW == NULL) {
            RegCloseKey(hKey);
            return ERROR_NO_UNICODE_TRANSLATION;
        }
        lReturnValue = RegSetValueExW(hKey, SERVICES_REG_ENVIRONMENT_W, 0, REG_MULTI_SZ, (const BYTE *) dataW, (DWORD)(dataW_len * sizeof(wchar_t)));
        efree(dataW);
    } else {
        *step = "Error remove key";
        lReturnValue = RegDeleteValueW(hKey, SERVICES_REG_ENVIRONMENT_W);
    }
    RegCloseKey(hKey);
    return lReturnValue;
}

void get_service_registry_key(char * service, int service_len, char ** service_key, int * service_key_len) {

    const char *prefix = SERVICES_REG_KEY_ROOT;
    *service_key_len = (strlen(prefix) + service_len + 1);
    *service_key = emalloc(sizeof(char) * *service_key_len);

    sprintf(*service_key, "%s%.*s", prefix, (int)service_len, service);
}

void add_or_replace_environment_value(char *data, int data_len, const char *env_name, const char *new_value, char ** new_data, int * new_data_len) {
    if (data == NULL || data_len == 0) {
        *new_data_len = strlen(env_name) + 1 + strlen(new_value) + 2;
        *new_data = emalloc(*new_data_len);

        sprintf(*new_data, "%s=%s", env_name, new_value);

        (*new_data)[*new_data_len - 2] = '\0'; // Ajouter le premier \0
        (*new_data)[*new_data_len - 1] = '\0'; // Ajouter le second \0

        return;
    }

    size_t offset = 0;
    int found = 0;

    // Calculer la longueur de la nouvelle chaîne finale si besoin de réallocation
    *new_data_len = data_len + strlen(new_value) + 1;
    *new_data = emalloc(*new_data_len);

    size_t new_offset = 0;

    while (offset < data_len) {
        char *sub_string = data + offset;
        size_t sub_string_len = strlen(sub_string);

        // Si la sous-chaîne est vide, c'est la fin de la chaîne principale (double \0)
        if (sub_string_len == 0) {
            break;
        }

        char *equal_sign = strchr(sub_string, '=');

        if (equal_sign != NULL) {
            *equal_sign = '\0'; // Séparer la clé et la valeur temporairement

            // Vérifier si la clé correspond à "env"
            if (strcmp(sub_string, env_name) == 0) {
                // Construire la nouvelle sous-chaîne avec la nouvelle valeur
                new_offset += sprintf(*new_data + new_offset, "%s=%s", sub_string, new_value);
                found = 1;
            } else {
                // Copier la sous-chaîne telle quelle dans le nouvel espace mémoire
                new_offset += sprintf(*new_data + new_offset, "%s=%s", sub_string, equal_sign + 1);
            }

            *equal_sign = '='; // Restaurer le '='
        } else {
            if (strcmp(sub_string, env_name) == 0) {
                new_offset += sprintf(*new_data + new_offset, "%s=%s", sub_string, new_value);
                found = 1;
            } else {
                // Copier la sous-chaîne sans '=' telle quelle
                new_offset += sprintf(*new_data + new_offset, "%s", sub_string);
            }
        }

        (*new_data)[new_offset++] = '\0'; // Ajouter un '\0' après chaque sous-chaîne
        offset += sub_string_len + 1;
    }

    if (!found) {
        // Ajouter la nouvelle sous-chaîne à la fin si "env" n'a pas été trouvé
        new_offset += sprintf(*new_data + new_offset, "%s=%s", env_name, new_value);
    }

    // Ajouter un double '\0' pour terminer la chaîne
    (*new_data)[new_offset++] = '\0';
    (*new_data)[new_offset++] = '\0';

    *new_data_len = new_offset;
}

void remove_environment_value(char *data, int data_len, const char *env_name, char ** new_data, int * new_data_len) {
    if (data == NULL || data_len == 0) {
        return;
    }

    size_t offset = 0;

    // Calculer la longueur de la nouvelle chaîne finale si besoin de réallocation
    *new_data_len = data_len + 1;
    *new_data = emalloc(*new_data_len);

    size_t new_offset = 0;

    while (offset < data_len) {
        char *sub_string = data + offset;
        size_t sub_string_len = strlen(sub_string);

        // Si la sous-chaîne est vide, c'est la fin de la chaîne principale (double \0)
        if (sub_string_len == 0) {
            break;
        }

        char *equal_sign = strchr(sub_string, '=');

        if (equal_sign != NULL) {
            *equal_sign = '\0'; // Séparer la clé et la valeur temporairement

            // Vérifier si la clé correspond à "env"
            if (strcmp(sub_string, env_name) != 0) {
                // Copier la sous-chaîne telle quelle dans le nouvel espace mémoire
                new_offset += sprintf(*new_data + new_offset, "%s=%s\0", sub_string, equal_sign + 1);
            }

            *equal_sign = '='; // Restaurer le '='
        } else {
            if (strcmp(sub_string, env_name) != 0) {
                // Copier la sous-chaîne sans '=' telle quelle
                new_offset += sprintf(*new_data + new_offset, "%s\0", sub_string);
            }
        }
        offset += sub_string_len + 1;
    }
    if (new_offset > 1) {
        // Ajouter un double '\0' pour terminer la chaîne
        (*new_data)[new_offset++] = '\0';
    }
    *new_data_len = new_offset;
}

long set_service_base_priority(char *service, int service_len, int priority) {
    HKEY hKey;
    LONG lReturnValue = win32_internal_open_service_key(service, service_len, &hKey);
    DWORD value = (DWORD) priority;

    if (lReturnValue != ERROR_SUCCESS) {
        return lReturnValue;
    }

    lReturnValue = RegSetValueExW(hKey, SERVICES_REG_BASE_PRIORITY_W, 0, REG_DWORD, (CONST BYTE *)&value, sizeof(value));

    RegCloseKey(hKey);

    return lReturnValue;
}


long get_service_base_priority(char *service, int service_len, int *priority) {
    HKEY hKey;
    LONG lReturnValue = win32_internal_open_service_key(service, service_len, &hKey);
    DWORD dwType = REG_DWORD;
    DWORD dwSize = sizeof(DWORD);

    if (lReturnValue != ERROR_SUCCESS) {
        return lReturnValue;
    }

    lReturnValue = RegQueryValueExW(hKey, SERVICES_REG_BASE_PRIORITY_W, 0, &dwType, (LPBYTE)priority, &dwSize);

    RegCloseKey(hKey);

    return lReturnValue;
}
