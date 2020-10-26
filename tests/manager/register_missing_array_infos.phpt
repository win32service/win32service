--TEST--
Test function win32_create_service arguments
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip only windows test.');
?>
--FILE--
<?php

function displayException(Throwable $e){
    printf("%s: %s\n", get_class($e), $e->getMessage());
}

$service = [
];
try {
    var_dump(win32_create_service($service));
} catch (Throwable $e) {
    displayException($e);
}
$service[WIN32_INFO_SERVICE] = '';

try {
    var_dump(win32_create_service($service));
} catch (Throwable $e) {
    displayException($e);
}
$service[WIN32_INFO_SERVICE] = 'WindowsServicePhpTest';
$service[WIN32_INFO_DISPLAY] = 'Windows service PHP test';

//try {
//    var_dump(win32_create_service($service));
//} catch (Throwable $e) {
//    displayException($e);
//}
$service[WIN32_INFO_DESCRIPTION] = 'This service is an PHP example for test';

try {
    var_dump(win32_create_service($service));
} catch (Throwable $e) {
    displayException($e);
}

$service[WIN32_INFO_PATH] = '"' . dirname(PHP_BINARY) . '\\php-win.exe"';

//try {
//    var_dump(win32_create_service($service));
//} catch (Throwable $e) {
//    displayException($e);
//}

$service[WIN32_INFO_PARAMS] = '"' . __FILE__ . '" run';

//try {
//    var_dump(win32_create_service($service));
//} catch (Throwable $e) {
//    displayException($e);
//}

$service[WIN32_INFO_SVC_TYPE] = 0x00000001;
try {
    var_dump(win32_create_service($service));
} catch (Throwable $e) {
    displayException($e);
}
unset($service[WIN32_INFO_SVC_TYPE]);

$service[WIN32_INFO_START_TYPE] = 0x00000008;
try {
    var_dump(win32_create_service($service));
} catch (Throwable $e) {
    displayException($e);
}
unset($service[WIN32_INFO_START_TYPE]);

$service[WIN32_INFO_ERROR_CONTROL] = 0x00000008;
try {
    var_dump(win32_create_service($service));
} catch (Throwable $e) {
    displayException($e);
}
unset($service[WIN32_INFO_ERROR_CONTROL]);

$service[WIN32_INFO_BASE_PRIORITY] = 0x00000008;
try {
    var_dump(win32_create_service($service));
} catch (Throwable $e) {
    displayException($e);
}
unset($service[WIN32_INFO_BASE_PRIORITY]);

$service[WIN32_INFO_RECOVERY_DELAY] = -10;
try {
    var_dump(win32_create_service($service));
} catch (Throwable $e) {
    displayException($e);
}
$service[WIN32_INFO_RECOVERY_DELAY] = 10;
$service[WIN32_INFO_RECOVERY_ACTION_1] = 0x00000004;
try {
    var_dump(win32_create_service($service));
} catch (Throwable $e) {
    displayException($e);
}
$service[WIN32_INFO_RECOVERY_ACTION_1] = 0x00000000;
$service[WIN32_INFO_RECOVERY_ACTION_2] = 0x00000004;
try {
    var_dump(win32_create_service($service));
} catch (Throwable $e) {
    displayException($e);
}
$service[WIN32_INFO_RECOVERY_ACTION_2] = 0x00000000;
$service[WIN32_INFO_RECOVERY_ACTION_3] = 0x00000004;
try {
    var_dump(win32_create_service($service));
} catch (Throwable $e) {
    displayException($e);
}
$service[WIN32_INFO_RECOVERY_ACTION_3] = 0x00000000;
$service[WIN32_INFO_RECOVERY_RESET_PERIOD] = -10;
try {
    var_dump(win32_create_service($service));
} catch (Throwable $e) {
    displayException($e);
}
?>
--EXPECTF--
ValueError: win32_create_service(): Argument #1 ($details) the value for key 'service' is wrong
ValueError: win32_create_service(): Argument #1 ($details) the value for key 'service' cannot be empty
ValueError: win32_create_service(): Argument #1 ($details) the value for key 'path' is wrong
ValueError: win32_create_service(): Argument #1 ($details) the value %d for 'svc_type' key is wrong, Use WIN32_SERVICE_WIN32_OWN_PROCESS, WIN32_SERVICE_INTERACTIVE_PROCESS or WIN32_SERVICE_WIN32_OWN_PROCESS_INTERACTIVE constants
ValueError: win32_create_service(): Argument #1 ($details) the value %d for 'start_type' key is wrong, Use WIN32_SERVICE_BOOT_START, WIN32_SERVICE_SYSTEM_START, WIN32_SERVICE_AUTO_START, WIN32_SERVICE_DEMAND_START or WIN32_SERVICE_DISABLED constants
ValueError: win32_create_service(): Argument #1 ($details) the value %d for 'error_control' key is wrong, Use WIN32_SERVICE_ERROR_IGNORE, WIN32_SERVICE_ERROR_NORMAL, WIN32_SERVICE_ERROR_SEVERE or WIN32_SERVICE_ERROR_CRITICAL constants
ValueError: win32_create_service(): Argument #1 ($details) the value %d for 'base_priority' key is wrong, Use WIN32_ABOVE_NORMAL_PRIORITY_CLASS, WIN32_BELOW_NORMAL_PRIORITY_CLASS, WIN32_HIGH_PRIORITY_CLASS, WIN32_IDLE_PRIORITY_CLASS, WIN32_NORMAL_PRIORITY_CLASS or WIN32_REALTIME_PRIORITY_CLASS constants
ValueError: win32_create_service(): Argument #1 ($details) the value for key 'recovery_delay' must between 0 and %d. Got -%d.
ValueError: win32_create_service(): Argument #1 ($details) the value %d for 'recovery_action_1' key is wrong. Use WIN32_SC_ACTION_NONE, WIN32_SC_ACTION_REBOOT, WIN32_SC_ACTION_RESTART or WIN32_SC_ACTION_RUN_COMMAND constants
ValueError: win32_create_service(): Argument #1 ($details) the value %d for 'recovery_action_2' key is wrong. Use WIN32_SC_ACTION_NONE, WIN32_SC_ACTION_REBOOT, WIN32_SC_ACTION_RESTART or WIN32_SC_ACTION_RUN_COMMAND constants
ValueError: win32_create_service(): Argument #1 ($details) the value %d for 'recovery_action_3' key is wrong. Use WIN32_SC_ACTION_NONE, WIN32_SC_ACTION_REBOOT, WIN32_SC_ACTION_RESTART or WIN32_SC_ACTION_RUN_COMMAND constants
ValueError: win32_create_service(): Argument #1 ($details) the value for key 'recovery_reset_period' must between 0 and %d. Got -%d.
