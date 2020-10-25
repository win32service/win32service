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
--EXPECT--
Fatal error: Uncaught TypeError: win32_create_service(): Argument #1 ($details) must be of type array, stdClass given in C:\projects\php-src\ext\win32service\tests\manager\register_bad_arg.php:2
Stack trace:
#0 C:\projects\php-src\ext\win32service\tests\manager\register_bad_arg.php(2): win32_create_service(Object(stdClass))
#1 {main}
  thrown in C:\projects\php-src\ext\win32service\tests\manager\register_bad_arg.php on line 2
