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
		WIN32_INFO_SERVICE => 'WindowsServicePhpTest',
		WIN32_INFO_DISPLAY => 'Windows service PHP test',
		WIN32_INFO_DESCRIPTION => 'This service is an PHP example for test',
		WIN32_INFO_PATH => '"' . dirname(PHP_BINARY) . '\\php-win.exe"',
		WIN32_INFO_PARAMS => '"' . __FILE__ . '" run',
		WIN32_INFO_START_TYPE => WIN32_SERVICE_AUTO_START,
		WIN32_INFO_BASE_PRIORITY => WIN32_BELOW_NORMAL_PRIORITY_CLASS,
];
if (win32_exists_service($service[WIN32_INFO_SERVICE])) {
    win32_delete_service($service[WIN32_INFO_SERVICE]);
}
 var_dump(win32_create_service($service));
try {
    var_dump(win32_create_service($service));
} catch (Throwable $e) {
    displayException($e);
}
sleep(5);
echo "Set priority\n";
var_dump(win32_set_service_priority($service[WIN32_INFO_SERVICE], WIN32_NORMAL_PRIORITY_CLASS));
echo "Read right\n";
var_dump(win32_read_right_access_service($service[WIN32_INFO_SERVICE], 'subuser'));
sleep(1);
echo "add right\n";
var_dump(win32_add_right_access_service($service[WIN32_INFO_SERVICE], 'subuser',  WIN32_SERVICE_START | WIN32_SERVICE_STOP | WIN32_READ_CONTROL));
sleep(1);
echo "Read right\n";
var_dump($infos = win32_read_right_access_service($service[WIN32_INFO_SERVICE], 'subuser'));
var_dump($infos->getFullUsername());
var_dump($infos->getUsername());
var_dump($infos->getDomain());
var_dump($infos->getRights());
var_dump($infos->isGrantAccess());
var_dump($infos->isDenyAccess());
var_dump((string) $infos);
sleep(1);
echo "read all rights\n";
var_dump($infoAll = win32_read_all_rights_access_service($service[WIN32_INFO_SERVICE]));
foreach ($infoAll as $info) {
	var_dump($info->getFullUsername());
	var_dump($info->getUsername());
	var_dump($info->getDomain());
	var_dump($info->getRights());
	var_dump($info->isGrantAccess());
	var_dump($info->isDenyAccess());
	var_dump((string) $info);
}

sleep(1);
echo "remove right\n";
var_dump(win32_remove_right_access_service($service[WIN32_INFO_SERVICE], 'subuser'));
sleep(1);
echo "Read right\n";
var_dump(win32_read_right_access_service($service[WIN32_INFO_SERVICE], 'subuser'));
sleep(5);
var_dump(win32_delete_service($service[WIN32_INFO_SERVICE]));
//sleep(5);
try {
    var_dump(win32_delete_service($service[WIN32_INFO_SERVICE]));
} catch (Throwable $e) {
    displayException($e);
}
?>
--EXPECTF--
NULL
Win32ServiceException: (1073) Error service exists (on create service)
Set priority
NULL
Read right
object(Win32Service\RightInfo)#2 (0) {
}
add right
NULL
Read right
object(Win32Service\RightInfo)#2 (0) {
}
string(7) "subuser"
string(7) "subuser"
NULL
array(3) {
  [16]=>
  string(13) "SERVICE_START"
  [32]=>
  string(12) "SERVICE_STOP"
  [131072]=>
  string(12) "READ_CONTROL"
}
bool(true)
bool(false)
string(16) "subuser 131120 1"
read all rights
array(5) {
  [0]=>
  object(Win32Service\RightInfo)#3 (0) {
  }
  [1]=>
  object(Win32Service\RightInfo)#4 (0) {
  }
  [2]=>
  object(Win32Service\RightInfo)#5 (0) {
  }
  [3]=>
  object(Win32Service\RightInfo)#6 (0) {
  }
  [4]=>
  object(Win32Service\RightInfo)#7 (0) {
  }
}
string(%d) "%s\subuser"
string(7) "subuser"
string(%d) "%s"
array(3) {
  [16]=>
  string(13) "SERVICE_START"
  [32]=>
  string(12) "SERVICE_STOP"
  [131072]=>
  string(12) "READ_CONTROL"
}
bool(true)
bool(false)
string(%d) "%s\subuser 131120 1"
string(19) "NT AUTHORITY\SYSTEM"
string(6) "SYSTEM"
string(12) "NT AUTHORITY"
array(9) {
  [8]=>
  string(28) "SERVICE_ENUMERATE_DEPENDENTS"
  [128]=>
  string(19) "SERVICE_INTERROGATE"
  [64]=>
  string(22) "SERVICE_PAUSE_CONTINUE"
  [1]=>
  string(20) "SERVICE_QUERY_CONFIG"
  [4]=>
  string(20) "SERVICE_QUERY_STATUS"
  [16]=>
  string(13) "SERVICE_START"
  [32]=>
  string(12) "SERVICE_STOP"
  [256]=>
  string(28) "SERVICE_USER_DEFINED_CONTROL"
  [131072]=>
  string(12) "READ_CONTROL"
}
bool(true)
bool(false)
string(28) "NT AUTHORITY\SYSTEM 131581 1"
string(22) "BUILTIN\Administrators"
string(14) "Administrators"
string(7) "BUILTIN"
array(14) {
  [983551]=>
  string(18) "SERVICE_ALL_ACCESS"
  [2]=>
  string(21) "SERVICE_CHANGE_CONFIG"
  [8]=>
  string(28) "SERVICE_ENUMERATE_DEPENDENTS"
  [128]=>
  string(19) "SERVICE_INTERROGATE"
  [64]=>
  string(22) "SERVICE_PAUSE_CONTINUE"
  [1]=>
  string(20) "SERVICE_QUERY_CONFIG"
  [4]=>
  string(20) "SERVICE_QUERY_STATUS"
  [16]=>
  string(13) "SERVICE_START"
  [32]=>
  string(12) "SERVICE_STOP"
  [256]=>
  string(28) "SERVICE_USER_DEFINED_CONTROL"
  [65536]=>
  string(6) "DELETE"
  [131072]=>
  string(12) "READ_CONTROL"
  [262144]=>
  string(9) "WRITE_DAC"
  [524288]=>
  string(11) "WRITE_OWNER"
}
bool(true)
bool(false)
string(31) "BUILTIN\Administrators 983551 1"
string(24) "NT AUTHORITY\INTERACTIVE"
string(11) "INTERACTIVE"
string(12) "NT AUTHORITY"
array(6) {
  [8]=>
  string(28) "SERVICE_ENUMERATE_DEPENDENTS"
  [128]=>
  string(19) "SERVICE_INTERROGATE"
  [1]=>
  string(20) "SERVICE_QUERY_CONFIG"
  [4]=>
  string(20) "SERVICE_QUERY_STATUS"
  [256]=>
  string(28) "SERVICE_USER_DEFINED_CONTROL"
  [131072]=>
  string(12) "READ_CONTROL"
}
bool(true)
bool(false)
string(33) "NT AUTHORITY\INTERACTIVE 131469 1"
string(20) "NT AUTHORITY\SERVICE"
string(7) "SERVICE"
string(12) "NT AUTHORITY"
array(6) {
  [8]=>
  string(28) "SERVICE_ENUMERATE_DEPENDENTS"
  [128]=>
  string(19) "SERVICE_INTERROGATE"
  [1]=>
  string(20) "SERVICE_QUERY_CONFIG"
  [4]=>
  string(20) "SERVICE_QUERY_STATUS"
  [256]=>
  string(28) "SERVICE_USER_DEFINED_CONTROL"
  [131072]=>
  string(12) "READ_CONTROL"
}
bool(true)
bool(false)
string(29) "NT AUTHORITY\SERVICE 131469 1"
remove right
NULL
Read right
object(Win32Service\RightInfo)#8 (0) {
}
NULL
Win32ServiceException: (1060) Error service does not exist ()