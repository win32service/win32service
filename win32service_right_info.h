#include "php.h"

extern zend_class_entry *win32service_right_info_class_entry;

void win32service_create_right_info(zval *return_value, char * domain, long domain_len, char * username, long username_len, long mask, long accessMode);
char * win32service_right_info_getFullname(win32service_right_info_object * rIObject);

PHP_MINIT_FUNCTION(win32service_right_info);
