--TEST--
Test win32_query_service_config and win32_update_service_config
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip only windows test.');
?>
--FILE--
<?php
function displayException(Throwable $e){
    printf("%s: (%d) %s\n", get_class($e), $e->getCode(), $e->getMessage());
}

$serviceName = 'Win32ServiceTestConfig';
$service = [
    'service' => $serviceName,
    'display' => 'Win32Service Test Config Service',
    'description' => 'Test description for win32service config',
    'path' => '"' . PHP_BINARY . '"',
    'params' => __FILE__ . ' run',
    'start_type' => WIN32_SERVICE_DEMAND_START,
];

if (win32_exists_service($serviceName)) {
    win32_delete_service($serviceName);
}

echo "Creating service\n";
win32_create_service($service);

echo "Querying initial config\n";
$config = win32_query_service_config($serviceName);
var_dump($config['service']);
var_dump($config['display']);
var_dump($config['description']);
var_dump($config['start_type']);

echo "Updating config\n";
$update = [
    'display' => 'Updated Win32Service Display Name',
    'description' => 'Updated description',
    'start_type' => WIN32_SERVICE_AUTO_START,
];
win32_update_service_config($serviceName, $update);

echo "Querying updated config\n";
$config = win32_query_service_config($serviceName);
var_dump($config['display']);
var_dump($config['description']);
var_dump($config['start_type']);
unset($config);

echo "Updating more config fields\n";
$updateMore = [
    'path' => '"' . PHP_BINARY . '"',
    'params' => __FILE__ . ' run-updated',
    'recovery_delay' => 120000,
    'recovery_action_1' => WIN32_SC_ACTION_RESTART,
    'recovery_action_2' => WIN32_SC_ACTION_RUN_COMMAND,
    'recovery_command' => 'echo "service failed"',
];
win32_update_service_config($serviceName, $updateMore);
sleep(1);
echo "Querying more updated config\n";
$config = win32_query_service_config($serviceName);
var_dump($config['path']);
var_dump($config['recovery_delay']);
var_dump($config['recovery_action_1']);
var_dump($config['recovery_action_2']);
var_dump($config['recovery_command']);

sleep(1);
echo "Cleaning up\n";
win32_delete_service($serviceName);

?>
--EXPECTF--
Creating service
Querying initial config
string(22) "Win32ServiceTestConfig"
string(32) "Win32Service Test Config Service"
string(40) "Test description for win32service config"
int(3)
Updating config
Querying updated config
string(33) "Updated Win32Service Display Name"
string(19) "Updated description"
int(2)
Updating more config fields
Querying more updated config
string(%d) ""%s" %s run-updated"
int(120000)
int(1)
int(3)
string(21) "echo "service failed""
Cleaning up
