--TEST--
Test service registration
--FILE--
<?php

$service = [
	'run_file' => __DIR__ . '/service_run.log',
	'log_file' => __DIR__ . '/service_%Y%m%d.log',
	'loop_wait' => 10,
	'service' => [
		'service' => 'WindowsServicePhpTest',
		'display' => 'Windows service PHP test',
		'description' => 'This service is an PHP example for test',
		'path' => '"' . dirname(PHP_BINARY) . '\\php-win.exe"',
		'params' => '"' . __FILE__ . '" run',
		'start_type' => WIN32_SERVICE_AUTO_START,
	],
];

var_dump(win32_create_service($service));
?>
--EXPECT--
int(0)