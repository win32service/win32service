--TEST--
Test service registration with object
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip only windows test.');
?>
--FILE--
<?php
var_dump(win32_create_service(new stdClass()));
?>
--EXPECTF--
Fatal error: Uncaught TypeError: win32_create_service(): Argument #1 ($details) must be of type array, stdClass given in %s:%d
Stack trace:
#0 %s(%d): win32_create_service(Object(stdClass))
#1 {main}
  thrown in %s on line %d
