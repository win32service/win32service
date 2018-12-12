<?php
/**
 * This file is part of win32service extension package.
 * This file is an manager for Windows Service.
 *
 * Requirements : PHP 7+
 *
 */

namespace win32service;

// Extension loading check

if (!extension_loaded('win32service')) {
	throw new \Exception('The php_win32service.dll extension is not loaded ! Please configure it into your php configuration file.');
}

//Configuration
$service = [
	'run_file' => __DIR__ . '/service_run.log',
	'log_file' => __DIR__ . '/service_%Y%m%d.log',
	'loop_wait' => 10,
	'service' => [
		'service' => 'WindowsServicePhpObjectTest',
		'display' => 'Windows service PHP Object test',
		'description' => 'This service is an PHP example for test using Object',
		'path' => '"' . dirname(PHP_BINARY) . '\\php-win.exe"',
		'params' => '"' . __DIR__ . '\\WinService.php" run',
		'start_type' => WIN32_SERVICE_AUTO_START,
	],
];


/**
 * The common code for all object
 */
abstract class WinServiceAbstract {

	protected $paused = false;
	protected $service = null;
	protected $status = null;
	protected $config = null;
	protected $cmd = null;
	protected $commands = null;

	public function __construct($service, $cmd = null)
	{
		$this->service = $service;
		$this->commands = $this->allowedCommand();
		$this->cmd = $cmd = strtolower($cmd);
		$this->write_log('INFO: Analyzing command', empty($cmd) or $cmd != 'run');
		if (empty($cmd) or !in_array($cmd, $this->commands)) {
			$this->write_log('ERROR: Invalid or missing command: ' . var_export($cmd, true), true);
			die (__FILE__ . ' ' . implode('|', $this->commands));
		}
                if (!method_exists($this, $cmd)) {
			$this->write_log('ERROR: Command not implemented: ' . $cmd, true);
			return;
		}
		$this->write_log('INFO: Querying service status', $cmd != 'run');
		$this->update_service_status();
		$this->write_log('INFO: Executing command: ' . $cmd, $cmd != 'run');
		$this->$cmd();
	}

	public function __destruct()
	{
	}

    private function update_service_status()
    {
        $this->status = win32_query_service_status($this->service['service']['service']);
    }

	protected function write_log($msg = null, $cmd = false)
	{
		if (!$cmd and $this->cmd != 'debug') {
			file_put_contents(strftime($this->service['log_file']), date('Y-m-d H:i:s') . "\t" . $msg . "\n", FILE_APPEND);
		} else {
			echo $msg, "\n";
		}
	}

	
	protected function win32_op_service($win32_op_service, $param, $cond = true, $msg = null, $debug = false)
	{
		$err_code = $win32_op_service($param);
		if ($cond === $err_code) {
			if (isset($msg)) {
				$this->write_log($msg, $debug);
			}
			return true;
		} elseif (false === $err_code) {
			$this->write_log('ERROR: Problem with the parameters', $debug);
		} elseif (WIN32_ERROR_ACCESS_DENIED === $err_code) {
			$this->write_log('ERROR: Access denied', $debug);
		} else {
			$this->write_log('ERROR: Win32 Error Code ' . $err_code, $debug);
		}
		return false;
	}


	protected function debug()
	{
		if ((!isset($this->status['CurrentState']) and $this->status == WIN32_ERROR_SERVICE_DOES_NOT_EXIST) or (isset($this->status['CurrentState']) and $this->status['CurrentState'] == WIN32_SERVICE_STOPPED)) {
			$this->write_log('DEBUG Start', true);
			$this->main_loop(true);
			$this->write_log('DEBUG End', true);
		}
	}


	/**
	 * @return array
	 */
	abstract protected function allowedCommand();
}
