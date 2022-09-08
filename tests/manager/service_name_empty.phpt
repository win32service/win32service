--TEST--
Test function with service name as empty string
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip only windows test.');
?>
--FILE--
<?php
function displayException(Throwable $e){
    printf("%s: (%d) %s\n", get_class($e), $e->getCode(), $e->getMessage());
}

$functions = [
              'win32_query_service_status',
              'win32_start_service',
              'win32_stop_service',
              'win32_pause_service',
              'win32_continue_service',
              ];

foreach ($functions as $function) {
printf("%s\n", $function);
    try {
        $function(new stdClass());
    } catch (Throwable $e) {
        displayException($e);
    }

    try {
        $function('');
    } catch (Throwable $e) {
        displayException($e);
    }
    try {
        var_dump($function('service'));
    } catch (Throwable $e) {
        displayException($e);
    }
}
echo "win32_send_custom_control\n";
try {
        win32_send_custom_control(new stdClass(), 200);
    } catch (Throwable $e) {
        displayException($e);
    }

    try {
        win32_send_custom_control('', 200);
    } catch (Throwable $e) {
        displayException($e);
    }
    try {
        win32_send_custom_control('service', 2);
    } catch (Throwable $e) {
        displayException($e);
    }
    try {
        var_dump(win32_send_custom_control('service', 200));
    } catch (Throwable $e) {
        displayException($e);
    }
?>
--EXPECTF--
win32_query_service_status
TypeError: (0) win32_query_service_status(): Argument #1 ($servicename) must be of type string, stdClass given
ValueError: (0) win32_query_service_status(): Argument #1 ($servicename) the value cannot be empty
Win32ServiceException: (1060) Error service does not exist ()
win32_start_service
TypeError: (0) win32_start_service(): Argument #1 ($servicename) must be of type string, stdClass given
ValueError: (0) win32_start_service(): Argument #1 ($servicename) the value cannot be empty
Win32ServiceException: (1060) Error service does not exist ()
win32_stop_service
TypeError: (0) win32_stop_service(): Argument #1 ($servicename) must be of type string, stdClass given
ValueError: (0) win32_stop_service(): Argument #1 ($servicename) the value cannot be empty
Win32ServiceException: (1060) Error service does not exist (on openning service)
win32_pause_service
TypeError: (0) win32_pause_service(): Argument #1 ($servicename) must be of type string, stdClass given
ValueError: (0) win32_pause_service(): Argument #1 ($servicename) the value cannot be empty
Win32ServiceException: (1060) Error service does not exist (on openning service)
win32_continue_service
TypeError: (0) win32_continue_service(): Argument #1 ($servicename) must be of type string, stdClass given
ValueError: (0) win32_continue_service(): Argument #1 ($servicename) the value cannot be empty
Win32ServiceException: (1060) Error service does not exist (on openning service)
win32_send_custom_control
TypeError: (0) win32_send_custom_control(): Argument #1 ($servicename) must be of type string, stdClass given
ValueError: (0) win32_send_custom_control(): Argument #1 ($servicename) the value cannot be empty
ValueError: (0) win32_send_custom_control(): Argument #2 ($control) the value must be between 128 and 255. Got 2
Win32ServiceException: (1060) Error service does not exist ()
