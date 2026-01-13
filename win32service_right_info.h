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

#include "php.h"

extern zend_class_entry *win32service_right_info_class_entry;

void win32service_create_right_info(zval *return_value, char * domain, long domain_len, char * username, long username_len, long mask, long accessMode);
char * win32service_right_info_getFullname(win32service_right_info_object * rIObject);

PHP_MINIT_FUNCTION(win32service_right_info);
