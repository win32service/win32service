--TEST--
Test ValueError argument positions and null bytes in win32_create_service details
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip only windows test.');
?>
--FILE--
<?php
function displayException(Throwable $e){
    printf("%s: (%d) %s\n", get_class($e), $e->getCode(), $e->getMessage());
}

$calls = [
    fn() => win32_add_right_access_service('', 'user', 1),
    fn() => win32_add_right_access_service('service', '', 1),
    fn() => win32_add_right_access_service('service', 'user', 0),
    fn() => win32_remove_right_access_service('', 'user'),
    fn() => win32_remove_right_access_service('service', ''),
    fn() => win32_create_service(['service' => 'svc', 'path' => "C:\\php.exe\0evil"]),
];
foreach ($calls as $call) {
    try {
        $call();
    } catch (Throwable $e) {
        displayException($e);
    }
}
?>
--EXPECT--
ValueError: (0) win32_add_right_access_service(): Argument #1 ($servicename) the value cannot be empty
ValueError: (0) win32_add_right_access_service(): Argument #2 ($username) the value cannot be empty
ValueError: (0) win32_add_right_access_service(): Argument #3 ($right) the value must be a combination of the WIN32_* rights constants
ValueError: (0) win32_remove_right_access_service(): Argument #1 ($servicename) the value cannot be empty
ValueError: (0) win32_remove_right_access_service(): Argument #2 ($username) the value cannot be empty
ValueError: (0) win32_create_service(): Argument #1 ($details) the value for key 'path' must not contain any null bytes
