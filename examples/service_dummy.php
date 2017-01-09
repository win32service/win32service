<?php
/**
 * This file is part of win32service extension package.
 * This file start or stop the "dummyphp" service according to their state.
 * Usage : php service_dummy.php
 *
 * Requirements : PHP 7+, Install service with file sample.php
 *
 */
 
if (!extension_loaded('win32service')) {
	throw new \Exception('The php_win32service.dll extension is not loaded ! Please configure it into your php configuration file.');
}

$service_name = 'dummyphp';

$status = win32_query_service_status($service_name);

switch ($status['CurrentState']) {
	case WIN32_SERVICE_STOPPED:
		echo "starting \n";
		$x = win32_start_service($service_name); // OK
		debug_zval_dump($x);
		break;
	case WIN32_SERVICE_STOP_PENDING:
		break;
	case WIN32_SERVICE_START_PENDING:
		break;
	case WIN32_SERVICE_RUNNING:
		echo "stopping \n";
		$x = win32_stop_service($service_name);
		debug_zval_dump($x);
		break;
	case WIN32_SERVICE_PAUSED:
		break;
	case WIN32_SERVICE_PAUSE_PENDING:
		break;
	case WIN32_SERVICE_CONTINUE_PENDING:
		break;
	default:
}
