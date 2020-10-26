--TEST--
Test win32_start_service_ctrl_dispatcher function argscal
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip only windows test.');
?>
--FILE--
<?php
function displayException(Throwable $e){
    printf("%s: (%d) %s\n", get_class($e), $e->getCode(), $e->getMessage());
}

try {
    win32_start_service_ctrl_dispatcher(new stdClass());
} catch (Throwable $e) {
    displayException($e);
}

try {
    win32_start_service_ctrl_dispatcher('');
} catch (Throwable $e) {
    displayException($e);
}
try {
    var_dump(win32_start_service_ctrl_dispatcher('service'));
} catch (Throwable $e) {
    displayException($e);
}


?>
--EXPECTF--
TypeError: (0) win32_start_service_ctrl_dispatcher(): Argument #1 ($name) must be of type string, stdClass given
ValueError: (0) win32_start_service_ctrl_dispatcher(): Argument #1 ($name) the value cannot be empty
Win32ServiceException: (1063) Error failed service controller connect
