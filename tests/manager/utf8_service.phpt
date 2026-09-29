--TEST--
Test service with non-ASCII (UTF-8) name, display name and description
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip only windows test.');
?>
--FILE--
<?php
$name = 'WindowsServicePhpTestÉté';
$service = [
    WIN32_INFO_SERVICE => $name,
    WIN32_INFO_DISPLAY => 'Service PHP de test é à ü 日本',
    WIN32_INFO_DESCRIPTION => 'Description avec accents é è ñ',
    WIN32_INFO_PATH => '"' . dirname(PHP_BINARY) . '\\php-win.exe"',
    WIN32_INFO_PARAMS => '"' . __FILE__ . '" run',
    WIN32_INFO_START_TYPE => WIN32_SERVICE_DEMAND_START,
];
if (win32_exists_service($name)) {
    win32_delete_service($name);
}
var_dump(win32_create_service($service));
var_dump(win32_exists_service($name));
$config = win32_query_service_config($name);
var_dump($config[WIN32_INFO_DISPLAY] === $service[WIN32_INFO_DISPLAY]);
var_dump($config[WIN32_INFO_DESCRIPTION] === $service[WIN32_INFO_DESCRIPTION]);
var_dump(win32_delete_service($name));
try {
    win32_exists_service("bad\xff");
} catch (Throwable $e) {
    echo get_class($e), ": ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
NULL
bool(true)
bool(true)
bool(true)
NULL
ValueError: A string argument must be valid UTF-8%A
