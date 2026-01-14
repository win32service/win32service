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

#ifndef WIN32SERVICE_WIN32SERVICE_REGISTRY_H
#define WIN32SERVICE_WIN32SERVICE_REGISTRY_H


#define SERVICES_REG_KEY_ROOT "SYSTEM\\CurrentControlSet\\Services\\"

#define SERVICES_REG_BASE_PRIORITY "BasePriority"
#define SERVICES_REG_ENVIRONMENT "Environment"

long get_service_environment_vars(char * service, int service_len, char ** data, int * data_length);
void get_service_registry_key(char * service, int service_len, char ** service_key, int * service_key_len);
void add_or_replace_environment_value(char *data, int data_len, const char *env_name, const char *new_value, char ** new_data, int * new_data_len);
void remove_environment_value(char *data, int data_len, const char *env_name, char ** new_data, int * new_data_len);

#endif //WIN32SERVICE_WIN32SERVICE_REGISTRY_H
