--TEST--
Test service running
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip only windows test.');
?>
--FILE--
<?php
function displayException(Throwable $e){
    printf("%s: (%d) %s\n", get_class($e), $e->getCode(), $e->getMessage());
}

var_dump(getenv(PHP_INI_SCAN_DIR));
phpinfo();
$service = [
		'service' => 'WindowsServicePhpTestRun',
		'display' => 'Windows service PHP test',
		'description' => 'This service is an PHP example for test',
		'path' => '"' . (PHP_BINARY) . '"',
		'params' => '"' . __DIR__ . '\\run.php"',
		'start_type' => WIN32_SERVICE_AUTO_START,
];
try {
var_dump(win32_create_service($service));
var_dump(win32_start_service($service['service']));
sleep(15);
var_dump(win32_query_service_status($service['service']));
var_dump(win32_stop_service($service['service']));
sleep(30);
var_dump(win32_delete_service($service['service']));
} catch (Throwable $e) {
  displayException($e);
}
if (is_readable(__DIR__.'/run.log')) {
echo file_get_contents(__DIR__.'/run.log');
}
?>
--EXPECTF--
NULL
NULL
array(9) {
  ["ServiceType"]=>
  int(16)
  ["CurrentState"]=>
  int(4)
  ["ControlsAccepted"]=>
  int(2047)
  ["Win32ExitCode"]=>
  int(0)
  ["ServiceSpecificExitCode"]=>
  int(0)
  ["CheckPoint"]=>
  int(0)
  ["WaitHint"]=>
  int(0)
  ["ProcessId"]=>
  int(%d)
  ["ServiceFlags"]=>
  int(0)
}
NULL
NULL
Win32ServiceException: (0) Service ctrl dispatcher already running in %s on line %d
