<?php
/**
 * This file is part of win32service extension package.
 * This file install or uninstall the "dummyphp" service.
 *
 * Requirements : PHP 7+
 *
 * Usage :
 *
 * php sample.php install
 * net start dummyphp
 * net stop dummyphp
 * php sample.php uninstall
 */

 if (!extension_loaded('win32service')) {
	throw new \Exception('The php_win32service.dll extension is not loaded ! Please configure it into your php configuration file.');
}
 
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
	die("bogus args");
}

$x = win32_start_service_ctrl_dispatcher('dummyphp');

win32_set_service_status(WIN32_SERVICE_RUNNING);

while (WIN32_SERVICE_CONTROL_STOP != win32_get_last_control_message()) {
	usleep(250000);
}
