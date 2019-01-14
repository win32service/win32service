--TEST--
Test service registration and unregistration
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip only windows test.');
?>
--FILE--
<?php

$service = [
		'service' => 'WindowsServicePhpTest',
		'display' => 'Windows service PHP test',
		'description' => 'This service is an PHP example for test',
		'path' => '"' . dirname(PHP_BINARY) . '\\php-win.exe"',
		'params' => '"' . __FILE__ . '" run',
		'start_type' => WIN32_SERVICE_AUTO_START,
];

var_dump(win32_create_service($service));
var_dump(win32_create_service($service));
var_dump(win32_delete_service($service['service']));
var_dump(win32_delete_service($service['service']));
?>
--EXPECT--
int(0)
int(1073)
int(0)
int(1060)
