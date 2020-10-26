--TEST--
Test service registration and unregistration
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
		'service' => 'WindowsServicePhpTest',
		'display' => 'Windows service PHP test',
		'description' => 'This service is an PHP example for test',
		'path' => '"' . dirname(PHP_BINARY) . '\\php-win.exe"',
		'params' => '"' . __FILE__ . '" run',
		'start_type' => WIN32_SERVICE_AUTO_START,
];

var_dump(win32_create_service($service));
try {
    var_dump(win32_create_service($service));
} catch (Throwable $e) {
    displayException($e);
}
var_dump(win32_delete_service($service['service']));
try {
    var_dump(win32_delete_service($service['service']));
} catch (Throwable $e) {
    displayException($e);
}
?>
--EXPECT--
int(0)
Win32ServiceException: (1073) Error service exists
int(0)
Win32ServiceException: (1060) Error service does not exist
