--TEST--
Test service environment variables management
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip only windows test.');
?>
--FILE--
<?php
function displayException(Throwable $e){
    printf("%s: (%d) %s\n", get_class($e), $e->getCode(), $e->getMessage());
}
$service = [
		'service' => 'WindowsServicePhpTestEnv',
		'display' => 'Windows service PHP Env test',
		'description' => 'This service is an PHP example with environment for test',
		'path' => '"' . dirname(PHP_BINARY) . '\\php-win.exe"',
		'params' => '"' . __FILE__ . '" run',
		'start_type' => WIN32_SERVICE_AUTO_START,
];
try {
	var_dump(win32_create_service($service));
} catch (\Win32ServiceException $e) {
	if ($e->getCode() !== WIN32_ERROR_SERVICE_EXISTS){
			displayException($e);
	} else {
		echo "NULL\n";
	}
}
sleep(3);
try {
    var_dump(win32_remove_service_env_var($service['service'], 'var1'));
} catch (Throwable $e) {
    displayException($e);
}
var_dump(win32_get_service_env_vars($service['service']));
var_dump(win32_add_service_env_var($service['service'], 'var1', 'val1'));
var_dump(win32_get_service_env_vars($service['service']));
var_dump(win32_add_service_env_var($service['service'], 'var2', 'val2'));
var_dump(win32_get_service_env_vars($service['service']));
var_dump(win32_remove_service_env_var($service['service'], 'var1'));
var_dump(win32_get_service_env_vars($service['service']));
var_dump(win32_remove_service_env_var($service['service'], 'var2'));
var_dump(win32_get_service_env_vars($service['service']));

var_dump(win32_delete_service($service['service']));
?>
--EXPECT--
NULL
Win32ServiceException: (2) Unknow error no 2 (Error on read service environment variables)
array(0) {
}
NULL
array(1) {
  ["var1"]=>
  string(4) "val1"
}
NULL
array(2) {
  ["var1"]=>
  string(4) "val1"
  ["var2"]=>
  string(4) "val2"
}
NULL
array(1) {
  ["var2"]=>
  string(4) "val2"
}
NULL
array(0) {
}
NULL