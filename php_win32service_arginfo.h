/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 01929bcf032688eb01f225aca403ba5b934f542d */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32_start_service_ctrl_dispatcher, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, gracefulExit, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32_set_service_pause_resume_state, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enable, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32_set_service_exit_mode, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, gracefulExit, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_win32_set_service_exit_code, 0, 0, long, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, exitCode, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32_set_service_status, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, status, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, checkpoint, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32_read_right_access_service, 0, 2, IS_OBJECT, 0)
	ZEND_ARG_TYPE_INFO(0, servicename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, username, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, machine, IS_STRING, 0, "\'\'")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32_read_all_rights_access_service, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, servicename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, machine, IS_STRING, 0, "\'\'")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32_add_right_access_service, 0, 3, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, servicename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, username, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, right, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, machine, IS_STRING, 0, "\'\'")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32_remove_right_access_service, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, servicename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, username, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, machine, IS_STRING, 0, "\'\'")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32_create_service, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, details, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, machine, IS_STRING, 0, "\'\'")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32_exists_service, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, servicename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, machine, IS_STRING, 0, "\'\'")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32_delete_service, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, servicename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, machine, IS_STRING, 0, "\'\'")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32_set_service_priority, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, servicename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, priority, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_win32_get_last_control_message, 0, 0, long, 0)
ZEND_END_ARG_INFO()

#define arginfo_win32_query_service_status arginfo_win32_read_all_rights_access_service

#define arginfo_win32_start_service arginfo_win32_delete_service

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32_get_service_env_vars, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, servicename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32_add_service_env_var, 0, 3, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, servicename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, varName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, varValue, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32_remove_service_env_var, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, servicename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, varName, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_win32_stop_service arginfo_win32_delete_service

#define arginfo_win32_pause_service arginfo_win32_delete_service

#define arginfo_win32_continue_service arginfo_win32_delete_service

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32_send_custom_control, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, servicename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, control, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, machine, IS_STRING, 0, "\'\'")
ZEND_END_ARG_INFO()

