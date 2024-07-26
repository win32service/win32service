//
// Created by jbnahan on 17/08/24.
//

#ifndef WIN32SERVICE_WIN32SERVICE_REGISTRY_H
#define WIN32SERVICE_WIN32SERVICE_REGISTRY_H


#define SERVICES_REG_KEY_ROOT "SYSTEM\\CurrentControlSet\\Services\\"

long get_service_environment_vars(char * service, int service_len, char ** data, int * data_length);
void get_service_registry_key(char * service, int service_len, char ** service_key, int * service_key_len);
void add_or_replace_environment_value(char *data, int data_len, const char *env_name, const char *new_value, char ** new_data, int * new_data_len);
void remove_environment_value(char *data, int data_len, const char *env_name, char ** new_data, int * new_data_len);

#endif //WIN32SERVICE_WIN32SERVICE_REGISTRY_H
