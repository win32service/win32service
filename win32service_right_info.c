
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "zend_exceptions.h"
#include "php_ini.h"
#include "php_win32service.h"
#include <aclapi.h>

zend_class_entry *win32service_right_info_class_entry;
#define this_ce win32service_right_info_class_entry

zend_object_handlers win32service_right_info_object_handlers;

void win32service_create_right_info(
        zval *return_value,
        char *domain,
        long domain_len,
        char *username,
        long username_len,
        long mask,
        long accessMode) {

//    printf(" win32service_create_right_info ");
    object_init_ex(return_value, this_ce);
//    printf(" init_ex ");
    win32service_right_info_object * object = php_win32service_right_info_object_fetch(Z_OBJ_P(return_value));

    if (domain_len > 0) {
//        printf(" domain:");
        object->domain = (char *) emalloc(sizeof(char) * domain_len + 1);
//        printf(" init ");
        strcpy(object->domain, domain);
//        printf(" copy");
    }

//    printf(" username");
    object->username = (char *) emalloc(sizeof(char) * username_len + 1);
//    printf(" init ");
    strcpy(object->username, username);
//    printf(" copy");

    object->mask = mask;
//    printf(" mask");
    object->accessMode = accessMode;
//    printf(" ok copy mode and data\n");
}

char *win32service_right_info_getFullname(win32service_right_info_object *rIObject) {
    if (rIObject->username == NULL) {
        return NULL;
    }

    if (rIObject->domain == NULL) {
        char *fullname = emalloc(sizeof(char) * strlen(rIObject->username)+1);
        strcpy(fullname, rIObject->username);
        return fullname;
    }

    char *fullname = emalloc(sizeof(char) * (strlen(rIObject->domain) + strlen(rIObject->username) + 2));
    strcpy(fullname, rIObject->domain);
    strcat(fullname, "\\");
    strcat(fullname, rIObject->username);
    return fullname;
}

static PHP_METHOD(win32service_right_info_class, __construct) {
    zend_throw_exception_ex(Win32ServiceException_ce_ptr, 16000, "Unable to call constructor from userland. Use win32_read_right_access_service or win32_read_all_rights_access_service to get instance.");
}

static PHP_METHOD(win32service_right_info_class, __clone) {
    zend_throw_exception_ex(Win32ServiceException_ce_ptr, 16000, "Unable to clone from userland. Use win32_read_right_access_service or win32_read_all_rights_access_service to get instance.");
}

static PHP_METHOD(win32service_right_info_class, __serialize) {

    zval * object = getThis();
    win32service_right_info_object * rIObject = php_win32service_right_info_object_fetch(Z_OBJ_P(object));
    array_init(return_value);
    if (rIObject->domain == NULL) {
        add_assoc_null(return_value, "domain");
    }else {
        add_assoc_string(return_value, "domain", rIObject->domain);
    }
    add_assoc_string(return_value, "username", rIObject->username);
    add_assoc_long(return_value, "mask", rIObject->mask);
    add_assoc_long(return_value, "accessMode", rIObject->accessMode);

}
// Debug function for HashTable
//static void print_hash_table(HashTable *ht) {
//    HashPosition pos;
//    zval *data;
//    zend_string *key;
//    zend_ulong idx;
//
//    zend_hash_internal_pointer_reset_ex(ht, &pos);
//
//    while ((data = zend_hash_get_current_data_ex(ht, &pos)) != NULL) {
//
//        if (zend_hash_get_current_key_ex(ht, &key, &idx, &pos) == HASH_KEY_IS_STRING) {
//            php_printf("Key: %s, Value: ", ZSTR_VAL(key));
//        } else {
//            php_printf("Key: %lu, Value: ", idx);
//        }
//
//        php_var_dump(data, 1);
//
//        zend_hash_move_forward_ex(ht, &pos);
//    }
//}


static PHP_METHOD(win32service_right_info_class, __unserialize) {
    HashTable *data;
    zval *object = getThis();
    win32service_right_info_object *rIObject = php_win32service_right_info_object_fetch(Z_OBJ_P(object));

    zval *domain, *username, *mask, *accessMode;

    // Parse the parameters
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "H", &data) == FAILURE) {
        return;
    }

    // Check and retrieve 'domain'
    if ((domain = zend_hash_str_find(data, "domain", sizeof("domain") - 1)) == NULL || (Z_TYPE_P(domain) != IS_STRING && Z_TYPE_P(domain) != IS_NULL)) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, 16000, "Missing or invalid 'domain' key");
        return;
    }

    // Check and retrieve 'username'
    if ((username = zend_hash_str_find(data, "username", sizeof("username") - 1)) == NULL || Z_TYPE_P(username) != IS_STRING) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, 16000, "Missing or invalid 'username' key");
        return;
    }

    // Check and retrieve 'mask'
    if ((mask = zend_hash_str_find(data, "mask", sizeof("mask") - 1)) == NULL || Z_TYPE_P(mask) != IS_LONG) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, 16000, "Missing or invalid 'mask' key");
        return;
    }

    // Check and retrieve 'accessMode'
    if ((accessMode = zend_hash_str_find(data, "accessMode", sizeof("accessMode") - 1)) == NULL || Z_TYPE_P(accessMode) != IS_LONG) {
        zend_throw_exception_ex(Win32ServiceException_ce_ptr, 16000, "Missing or invalid 'accessMode' key");
        return;
    }

    // Assign values to the structure fields
//    rIObject->domain = (char *) emalloc(sizeof(char) * domain_len + 1);
//    strcpy(object->domain, domain);
    if (Z_TYPE_P(domain) == IS_STRING) {
        rIObject->domain = estrdup(Z_STRVAL_P(domain));
    } else {
        rIObject->domain = NULL;
    }
    rIObject->username = estrdup(Z_STRVAL_P(username));
    rIObject->mask = Z_LVAL_P(mask);
    rIObject->accessMode = Z_LVAL_P(accessMode);
}

static PHP_METHOD(win32service_right_info_class, __toString) {
    zval * object = getThis();
    win32service_right_info_object * rIObject = php_win32service_right_info_object_fetch(Z_OBJ_P(object));
    char *fullname = win32service_right_info_getFullname(rIObject);
    char *value = emalloc(sizeof(char) * (strlen(fullname) + (snprintf(NULL, 0, "%ld", rIObject->mask) +
                                                              snprintf(NULL, 0, "%ld", rIObject->accessMode)) + 3));
    sprintf(value, "%s %ld %ld",
            fullname,
            rIObject->mask,
            rIObject->accessMode
    );

    ZVAL_STRING(return_value, value);
    efree(value);
}

static PHP_METHOD(win32service_right_info_class, getUsername) {
    zval * object = getThis();
    win32service_right_info_object * rIObject = php_win32service_right_info_object_fetch(Z_OBJ_P(object));

    if (rIObject->username == NULL) {
        RETURN_NULL();
    }

    RETURN_STRING(rIObject->username);
}

static PHP_METHOD(win32service_right_info_class, getDomain) {
    zval * object = getThis();
    win32service_right_info_object * rIObject = php_win32service_right_info_object_fetch(Z_OBJ_P(object));

    if (rIObject->domain == NULL) {
        RETURN_NULL();
    }

    RETURN_STRING(rIObject->domain);
}

static PHP_METHOD(win32service_right_info_class, getFullUsername) {
    zval * object = getThis();
    win32service_right_info_object * rIObject = php_win32service_right_info_object_fetch(Z_OBJ_P(object));

    char *result = win32service_right_info_getFullname(rIObject);

    if (result == NULL) {
        RETURN_NULL();
    }
    ZVAL_STRING(return_value, result);
    efree(result);
}

static PHP_METHOD(win32service_right_info_class, getRights) {
    zval * object = getThis();
    win32service_right_info_object * rIObject = php_win32service_right_info_object_fetch(Z_OBJ_P(object));

    long pAccess = rIObject->mask;// Z_LVAL_P(mask);
    array_init(return_value);

    if (((pAccess & GENERIC_ALL) == GENERIC_ALL)
        || ((pAccess & FILE_ALL_ACCESS) == FILE_ALL_ACCESS)) {
        add_index_string(return_value, GENERIC_ALL, "GENERIC_ALL");
    }
    if (((pAccess & GENERIC_READ) == GENERIC_READ)
        || ((pAccess & FILE_GENERIC_READ) == FILE_GENERIC_READ))
        add_index_string(return_value, GENERIC_READ, "GENERIC_READ");
    if (((pAccess & GENERIC_WRITE) == GENERIC_WRITE)
        || ((pAccess & FILE_GENERIC_WRITE) == FILE_GENERIC_WRITE))
        add_index_string(return_value, GENERIC_WRITE, "GENERIC_WRITE");
    if (((pAccess & GENERIC_EXECUTE) == GENERIC_EXECUTE)
        || ((pAccess & FILE_GENERIC_EXECUTE) == FILE_GENERIC_EXECUTE))
        add_index_string(return_value, GENERIC_EXECUTE, "GENERIC_EXECUTE");

    if ((pAccess & SERVICE_ALL_ACCESS) == SERVICE_ALL_ACCESS)
        add_index_string(return_value, SERVICE_ALL_ACCESS, "SERVICE_ALL_ACCESS");
    if ((pAccess & SERVICE_CHANGE_CONFIG) == SERVICE_CHANGE_CONFIG)
        add_index_string(return_value, SERVICE_CHANGE_CONFIG, "SERVICE_CHANGE_CONFIG");
    if ((pAccess & SERVICE_ENUMERATE_DEPENDENTS) == SERVICE_ENUMERATE_DEPENDENTS)
        add_index_string(return_value, SERVICE_ENUMERATE_DEPENDENTS, "SERVICE_ENUMERATE_DEPENDENTS");
    if ((pAccess & SERVICE_INTERROGATE) == SERVICE_INTERROGATE)
        add_index_string(return_value, SERVICE_INTERROGATE, "SERVICE_INTERROGATE");
    if ((pAccess & SERVICE_PAUSE_CONTINUE) == SERVICE_PAUSE_CONTINUE)
        add_index_string(return_value, SERVICE_PAUSE_CONTINUE, "SERVICE_PAUSE_CONTINUE");
    if ((pAccess & SERVICE_QUERY_CONFIG) == SERVICE_QUERY_CONFIG)
        add_index_string(return_value, SERVICE_QUERY_CONFIG, "SERVICE_QUERY_CONFIG");
    if ((pAccess & SERVICE_QUERY_STATUS) == SERVICE_QUERY_STATUS)
        add_index_string(return_value, SERVICE_QUERY_STATUS, "SERVICE_QUERY_STATUS");
    if ((pAccess & SERVICE_START) == SERVICE_START)
        add_index_string(return_value, SERVICE_START, "SERVICE_START");
    if ((pAccess & SERVICE_STOP) == SERVICE_STOP)
        add_index_string(return_value, SERVICE_STOP, "SERVICE_STOP");
    if ((pAccess & SERVICE_USER_DEFINED_CONTROL) == SERVICE_USER_DEFINED_CONTROL)
        add_index_string(return_value, SERVICE_USER_DEFINED_CONTROL, "SERVICE_USER_DEFINED_CONTROL");
    if ((pAccess & ACCESS_SYSTEM_SECURITY) == ACCESS_SYSTEM_SECURITY)
        add_index_string(return_value, ACCESS_SYSTEM_SECURITY, "ACCESS_SYSTEM_SECURITY");
    if ((pAccess & DELETE) == DELETE)
        add_index_string(return_value, DELETE, "DELETE");
    if ((pAccess & READ_CONTROL) == READ_CONTROL)
        add_index_string(return_value, READ_CONTROL, "READ_CONTROL");
    if ((pAccess & WRITE_DAC) == WRITE_DAC)
        add_index_string(return_value, WRITE_DAC, "WRITE_DAC");
    if ((pAccess & WRITE_OWNER) == WRITE_OWNER)
        add_index_string(return_value, WRITE_OWNER, "WRITE_OWNER");

}


static PHP_METHOD(win32service_right_info_class, isGrantAccess) {
    zval * object = getThis();
    win32service_right_info_object * rIObject = php_win32service_right_info_object_fetch(Z_OBJ_P(object));

    RETURN_BOOL((rIObject->accessMode & GRANT_ACCESS) == GRANT_ACCESS);
}

static PHP_METHOD(win32service_right_info_class, isDenyAccess) {
    zval * object = getThis();
    win32service_right_info_object * rIObject = php_win32service_right_info_object_fetch(Z_OBJ_P(object));

    RETURN_BOOL((rIObject->accessMode & DENY_ACCESS) == DENY_ACCESS);
}

zend_object *win32service_right_info_ctor(zend_class_entry *ce) {

//    printf(" ctor ");
    win32service_right_info_object * right_info =
            (win32service_right_info_object *) emalloc((sizeof(win32service_right_info_object) + zend_object_properties_size(ce)));

//    printf(" alloc obj ");
    zend_object_std_init(&right_info->zo, ce);
//    printf(" std init ");
    object_properties_init(&right_info->zo, ce);
//    printf(" properties init ");
    right_info->zo.handlers = &win32service_right_info_object_handlers;
//    printf(" handlers ");
    right_info->domain = NULL;
//    printf(" domain ");
    right_info->username = NULL;
//    printf(" username ");
    right_info->mask = 0;
//    printf(" mask ");
    right_info->accessMode = 0;
//    printf(" accessMode ");

    return &right_info->zo;
}

void win32service_right_info_free(zend_object *object) {
    win32service_right_info_object * right_info = php_win32service_right_info_object_fetch(object);
//    printf("get info ok \n");
    zend_object_std_dtor(&right_info->zo);
//    printf("obj ok \n");
    if (right_info->domain != NULL) {
        efree(right_info->domain);
//        printf("domain ok \n");
    }
    if (right_info->username != NULL) {
//        printf("username %s \n", right_info->username );
        efree(right_info->username);
//        printf("username ok \n");
    }

//    efree(right_info);

//    printf("free structure ok \n");
}

/* win32service_right_info_class ARG_INFO definition */
ZEND_BEGIN_ARG_INFO_EX(arginfo_win32service_right_info_class__construct, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32service_right_info_class__toString, ZEND_SEND_BY_VAL, 0, IS_STRING,
                                        0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32service_right_info_class__clone, ZEND_SEND_BY_VAL, 0, IS_VOID,
                                        0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32service_right_info_class__serialize, ZEND_SEND_BY_VAL, 0, IS_ARRAY,
                                        0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32service_right_info_class__unserialize, ZEND_SEND_BY_VAL, 1, IS_VOID,
                                        0)
    ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32service_right_info_class_getFullUsername, ZEND_SEND_BY_VAL, 0,
                                        IS_STRING, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32service_right_info_class_getRights, ZEND_SEND_BY_VAL, 0, IS_ARRAY,
                                        0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32service_right_info_class_isGrantAccess, ZEND_SEND_BY_VAL, 0,
                                        IS_LONG, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32service_right_info_class_isDenyAccess, ZEND_SEND_BY_VAL, 0,
                                        IS_LONG, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32service_right_info_class_getUsername, ZEND_SEND_BY_VAL, 0,
                                        IS_STRING, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32service_right_info_class_getDomain, ZEND_SEND_BY_VAL, 0, IS_STRING,
                                        0)
ZEND_END_ARG_INFO()

zend_function_entry win32service_right_info_class_functions[] = {
        PHP_ME
        (win32service_right_info_class, __construct, arginfo_win32service_right_info_class__construct, ZEND_ACC_PRIVATE)
        PHP_ME
        (win32service_right_info_class, __toString, arginfo_win32service_right_info_class__toString, ZEND_ACC_PUBLIC)
        PHP_ME
        (win32service_right_info_class, __clone, arginfo_win32service_right_info_class__clone, ZEND_ACC_PUBLIC)
        PHP_ME
        (win32service_right_info_class, __serialize, arginfo_win32service_right_info_class__serialize, ZEND_ACC_PUBLIC)
        PHP_ME
        (win32service_right_info_class, __unserialize, arginfo_win32service_right_info_class__unserialize, ZEND_ACC_PUBLIC)
        PHP_ME(win32service_right_info_class, getFullUsername, arginfo_win32service_right_info_class_getFullUsername,
               ZEND_ACC_PUBLIC)
        PHP_ME
        (win32service_right_info_class, getRights, arginfo_win32service_right_info_class_getRights, ZEND_ACC_PUBLIC)
        PHP_ME(win32service_right_info_class, isGrantAccess, arginfo_win32service_right_info_class_isGrantAccess,
               ZEND_ACC_PUBLIC)
        PHP_ME(win32service_right_info_class, isDenyAccess, arginfo_win32service_right_info_class_isDenyAccess,
               ZEND_ACC_PUBLIC)
        PHP_ME
        (win32service_right_info_class, getUsername, arginfo_win32service_right_info_class_getUsername, ZEND_ACC_PUBLIC)
        PHP_ME
        (win32service_right_info_class, getDomain, arginfo_win32service_right_info_class_getDomain, ZEND_ACC_PUBLIC)

        {NULL, NULL, NULL}
};


PHP_MINIT_FUNCTION (win32service_right_info) {
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Win32Service", "RightInfo", win32service_right_info_class_functions);
    ce.create_object = win32service_right_info_ctor;

    this_ce = zend_register_internal_class_ex(&ce, NULL);

    this_ce->ce_flags |= ZEND_ACC_FINAL;
#if PHP_VERSION_ID >= 80200
    this_ce->ce_flags |= ZEND_ACC_READONLY_CLASS | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
#endif

//    zend_declare_property_long(this_ce, ZEND_STRL("mask"), 0, ZEND_ACC_PUBLIC); //|ZEND_ACC_READONLY
//    zend_declare_property_long(this_ce, ZEND_STRL("accessMode"), 0, ZEND_ACC_PUBLIC); //|ZEND_ACC_READONLY
//    PHP_WIN32SERVICE_DECLARE_TYPED_PROPERTY_WITH_DEFAULT(this_ce, "username", ZEND_ACC_PUBLIC, IS_STRING, 1, ZVAL_NULL);
//    PHP_WIN32SERVICE_DECLARE_TYPED_PROPERTY_WITH_DEFAULT(this_ce, "domain", ZEND_ACC_PUBLIC, IS_STRING, 1, ZVAL_NULL);

    memcpy(&win32service_right_info_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    win32service_right_info_object_handlers.offset = XtOffsetOf(win32service_right_info_object, zo);
    win32service_right_info_object_handlers.free_obj = win32service_right_info_free;

    return SUCCESS;
}