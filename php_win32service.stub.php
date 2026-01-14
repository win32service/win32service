<?php


function win32_start_service_ctrl_dispatcher(string $name, bool $gracefulExit = true): bool {}

function win32_set_service_pause_resume_state(bool $enable = true): bool {}

function win32_set_service_exit_mode(bool $gracefulExit = true): bool {}

function win32_set_service_exit_code(int $exitCode = 0): long {}

function win32_set_service_status(int $status, int $checkpoint = 0): bool {}

function win32_read_right_access_service(string $servicename, string $username , string $machine = ''): object {}

function win32_read_all_rights_access_service(string $servicename , string $machine = ''): array {}

function win32_add_right_access_service(string $servicename, string $username, int $right, string $machine = ''): void {}

function win32_remove_right_access_service(string $servicename, string $username, string $machine = ''): void {}

function win32_create_service(array $details, string $machine = ''): void {}

function win32_exists_service(string $servicename, string $machine = ''): bool {}

function win32_delete_service(string $servicename, string $machine = ''): void {}

function win32_set_service_priority(string $servicename, int $priority): void {}

function win32_get_service_priority(string $servicename): int {}

function win32_get_last_control_message(): long {}

function win32_query_service_status(string $servicename, string $machine = ''): array {}

function win32_query_service_config(string $servicename, string $machine = ''): array {}

function win32_update_service_config(string $servicename, array $details, string $machine = ''): void {}

function win32_start_service(string $servicename, string $machine = ''): void {}

function win32_get_service_env_vars(string $servicename): array {}

function win32_add_service_env_var(string $servicename, string $varName, string $varValue): void {}

function win32_remove_service_env_var(string $servicename, string $varName): void {}

function win32_stop_service(string $servicename, string $machine = ''): void {}

function win32_pause_service(string $servicename, string $machine = ''): void {}

function win32_continue_service(string $servicename, string $machine = ''): void {}

function win32_send_custom_control(string $servicename, int $control, string $machine = ''): void {}

function win32_send_log(string $appname, int $eventCode, int $eventLevel, string $description): void {}