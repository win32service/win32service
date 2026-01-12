--TEST--
Test win32_query_service_config and win32_update_service_config with bad arguments
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip only windows test.');
?>
--FILE--
<?php
function displayException(Throwable $e){
    printf("%s: (%d) %s\n", get_class($e), $e->getCode(), $e->getMessage());
}

echo "win32_query_service_config with empty name\n";
try {
    win32_query_service_config('');
} catch (Throwable $e) {
    displayException($e);
}

echo "win32_query_service_config with non-existent service\n";
try {
    win32_query_service_config('NonExistentService');
} catch (Throwable $e) {
    displayException($e);
}

echo "win32_update_service_config with empty name\n";
try {
    win32_update_service_config('', []);
} catch (Throwable $e) {
    displayException($e);
}

echo "win32_update_service_config with non-existent service\n";
try {
    win32_update_service_config('NonExistentService', []);
} catch (Throwable $e) {
    displayException($e);
}

echo "win32_update_service_config with bad details type\n";
try {
    win32_update_service_config('service', new stdClass());
} catch (Throwable $e) {
    displayException($e);
}

?>
--EXPECTF--
win32_query_service_config with empty name
ValueError: (0) win32_query_service_config(): Argument #1 ($servicename) the value cannot be empty
win32_query_service_config with non-existent service
Win32ServiceException: (1060) Error service does not exist ()
win32_update_service_config with empty name
ValueError: (0) win32_update_service_config(): Argument #1 ($servicename) the value cannot be empty
win32_update_service_config with non-existent service
Win32ServiceException: (1060) Error service does not exist (on openning service)
win32_update_service_config with bad details type
TypeError: (0) win32_update_service_config(): Argument #2 ($details) must be of type array, stdClass given
