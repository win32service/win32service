--TEST--
Test service registration and unregistration
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip only windows test.');
?>
--FILE--
<?php
var_dump(win32_create_service(new stdClass()));
?>
--EXPECT--
PHP Warning:  win32_create_service() expects parameter 1 to be array, object given in php shell code on line 1

Warning: win32_create_service() expects parameter 1 to be array, object given in php shell code on line 1
bool(false)
