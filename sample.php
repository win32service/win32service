<?php
/* A sample service:
 *
 * php sample.php install
 * net start dummyphp
 * net stop dummyphp
 * php sample.php uninstall
 */

if ($argv[1] == 'install') {
	$x = win32_create_service(array(
		'service' => 'dummyphp',
		'display' => 'sample dummy PHP service',
		'params' => __FILE__ . ' run',
	));
	debug_zval_dump($x);
	exit;
} else if ($argv[1] == 'uninstall') {
	$x = win32_delete_service('dummyphp');
	debug_zval_dump($x);
	exit;
} else if ($argv[1] != 'run') {
	fwrite($fp, "BOGSU!\n");
	die("bogus args");
}

$x = win32_start_service_ctrl_dispatcher('dummyphp');

while (WIN32_SERVICE_CONTROL_STOP != win32_get_last_control_message()) {
	usleep(250000);
}

?>
