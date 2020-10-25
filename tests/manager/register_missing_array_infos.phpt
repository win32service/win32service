--TEST--
Test service registration and unregistration
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip only windows test.');
?>
--FILE--
<?php

$service = [
];

var_dump(win32_create_service($service));

$service['service'] = 'WindowsServicePhpTest';

var_dump(win32_create_service($service));
$service['display'] = 'Windows service PHP test';

var_dump(win32_create_service($service));
$service['description'] = 'This service is an PHP example for test';

var_dump(win32_create_service($service));
$service['path'] = '"' . dirname(PHP_BINARY) . '\\php-win.exe"';

var_dump(win32_create_service($service));
$service['params'] = '"' . __FILE__ . '" run';

var_dump(win32_create_service($service));
$service['start_type'] = 0x00000008;

var_dump(win32_create_service($service));
var_dump(win32_delete_service($service['service']));
?>
--EXPECT--
Warning: win32_create_service(): missing vital parameters in %s on line %d
bool(false)
int(1073)
int(1073)
int(1073)
int(1073)
int(87)
int(0)
int(0)
