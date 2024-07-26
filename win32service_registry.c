//
// Created by jbnahan on 17/08/24.
//

#include "win32service_registry.h"
#include "php.h"
#include <windows.h>
#include <winreg.h>


long get_service_environment_vars(char * service, int service_len, char ** pvData, int * pvDataLen) {

    HKEY hKey;
    LONG lReturnValue = 0;
    char * keyName = NULL;
    int keyNameLen = 0;
    get_service_registry_key(service, service_len, &keyName, &keyNameLen);

    lReturnValue = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyName, 0, KEY_ALL_ACCESS, &hKey);
    if (lReturnValue != ERROR_SUCCESS) {
        efree(keyName);
        if (lReturnValue == ERROR_FILE_NOT_FOUND) {
            return ERROR_SERVICE_DOES_NOT_EXIST;
        }

        return lReturnValue;
    }

    LPDWORD pdwType = NULL;

    lReturnValue = RegGetValue(hKey, NULL ,"Environment", RRF_RT_REG_MULTI_SZ|RRF_ZEROONFAILURE, pdwType, NULL, pvDataLen);
    if (lReturnValue != ERROR_SUCCESS) {
        efree(keyName);
        RegCloseKey(hKey);
        return lReturnValue;
    }
    *pvData = emalloc(sizeof(char) * *pvDataLen);

    lReturnValue = RegGetValue(hKey, NULL ,"Environment", RRF_RT_REG_MULTI_SZ|RRF_ZEROONFAILURE, pdwType, *pvData, pvDataLen);
    if (lReturnValue != ERROR_SUCCESS) {
        efree(keyName);
        RegCloseKey(hKey);
        return lReturnValue;
    }

    efree(keyName);
    RegCloseKey(hKey);
    return ERROR_SUCCESS;
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

