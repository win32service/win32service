--TEST--
Test function win32_delete_service with service name as empty string
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip only windows test.');
?>
--FILE--
<?php
function displayException(Throwable $e){
    printf("%s: (%d) %s\n", get_class($e), $e->getCode(), $e->getMessage());
}

echo "win32_delete_service\n";
try {
        win32_delete_service(new stdClass());
    } catch (Throwable $e) {
        displayException($e);
    }

    try {
        win32_delete_service('');
    } catch (Throwable $e) {
        displayException($e);
    }
    try {
        win32_delete_service('service');
    } catch (Throwable $e) {
        displayException($e);
    }
?>
--EXPECTF--
win32_delete_service
TypeError: (0) win32_delete_service(): Argument #1 ($servicename) must be of type string, stdClass given
ValueError: (0) win32_delete_service(): Argument #1 ($servicename) the value cannot be empty
Win32ServiceException: (1060) Error service does not exist ()
