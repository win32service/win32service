--TEST--
Test ValueError on empty service name or username in the read rights functions, and the WIN32_GENERIC_ALL constant
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
    fn() => win32_read_right_access_service('', 'user'),
    fn() => win32_read_right_access_service('service', ''),
    fn() => win32_read_all_rights_access_service(''),
    fn() => win32_add_right_access_service('service', 'user', WIN32_GENERIC_ALL << 4),
];
foreach ($calls as $call) {
    try {
        $call();
    } catch (Throwable $e) {
        displayException($e);
    }
}
var_dump(WIN32_GENERIC_ALL === 0x10000000);
?>
--EXPECT--
ValueError: (0) win32_read_right_access_service(): Argument #1 ($servicename) the value cannot be empty
ValueError: (0) win32_read_right_access_service(): Argument #2 ($username) the value cannot be empty
ValueError: (0) win32_read_all_rights_access_service(): Argument #1 ($servicename) the value cannot be empty
ValueError: (0) win32_add_right_access_service(): Argument #3 ($right) the value must be a combination of the WIN32_* rights constants
bool(true)
