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

$phpArgs = trim((string) getenv('TEST_PHP_ARGS'));
$service = [
		'service' => 'WindowsServicePhpTestRun',
		'display' => 'Windows service PHP test',
		'description' => 'This service is an PHP example for test',
		'path' => '"' . (PHP_BINARY) . '"',
		'params' => ($phpArgs !== '' ? $phpArgs . ' ' : '') . '"' . __DIR__ . '\\run.php"',
		'start_type' => WIN32_SERVICE_AUTO_START,
];
if (win32_exists_service($service['service'])) {
    win32_delete_service($service['service']);
}
@unlink(__DIR__.'/run.log');
try {
var_dump(win32_create_service($service));
var_dump(win32_start_service($service['service']));
sleep(15);
$status = win32_query_service_status($service['service']);
$status['ProcessId'] = 0;
var_dump($status);
var_dump(win32_stop_service($service['service']));
try {
    win32_pause_service($service['service']);
} catch (Win32ServiceException $e) {
    var_dump($e->getMessage());
}
sleep(30);
var_dump(win32_delete_service($service['service']));
} catch (Throwable $e) {
  displayException($e);
}
if (is_readable(__DIR__.'/run.log')) {
echo file_get_contents(__DIR__.'/run.log');
}
?>
--EXPECT--
NULL
NULL
array(9) {
  ["ServiceType"]=>
  int(16)
  ["CurrentState"]=>
  int(4)
  ["ControlsAccepted"]=>
  int(2045)
  ["Win32ExitCode"]=>
  int(0)
  ["ServiceSpecificExitCode"]=>
  int(0)
  ["CheckPoint"]=>
  int(0)
  ["WaitHint"]=>
  int(0)
  ["ProcessId"]=>
  int(0)
  ["ServiceFlags"]=>
  int(0)
}
NULL
string(50) "Error service cannot accept ctrl (on send control)"
NULL
pause/resume old state yes
pause/resume old state no
Win32ServiceException: (0) Service ctrl dispatcher already running
ValueError: (0) win32_set_service_pause_resume_state(): Argument #1 ($enable) Unable to change the pause/resume state when control dispatcher is already running. Call without argument if you want the state or call before win32_start_service_ctrl_dispatcher function.
