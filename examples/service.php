<?php
/**
 * This file is part of win32service extension package.
 * This file is an manager for Windows Service.
 *
 * Credit : alphp <https://github.com/alphp>
 *
 * Requirements : PHP 7+
 *
 * Usage :
 *
 * php service.php create
 * php service.php start
 * php service.php stop
 * php service.php pause
 * php service.php continue
 * php service.php debug
 * php service.php delete
 */

if (!extension_loaded('win32service')) {
	throw new \Exception('The php_win32service.dll extension is not loaded ! Please configure it into your php configuration file.');
}

class WinServiceManager {
	private $paused = false;
	private $service = null;
	private $status = null;
	private $config = null;
	private $cmd = null;
	private $commands = ['run', 'create', 'delete', 'stop', 'start', 'pause', 'continue', 'debug'];

	public function __construct($service, $cmd = null)
	{
		$this->service = $service;
		$this->cmd = $cmd = strtolower($cmd);
		$this->write_log('INFO: Analyzing command', empty($cmd) or $cmd != 'run');
		if (empty($cmd) or !in_array($cmd, $this->commands)) {
			$this->write_log('ERROR: Invalid or missing command: ' . var_export($cmd, true), true);
			die (__FILE__ . ' ' . implode('|', $this->commands));
		} elseif (!method_exists($this, $cmd)) {
			$this->write_log('ERROR: Command not implemented: ' . $cmd, true);
		} else {
			$this->write_log('INFO: Querying service status', $cmd != 'run');
			$this->update_service_status();
			$this->write_log('INFO: Executing command: ' . $cmd, $cmd != 'run');
			$this->$cmd();
		}
	}

	public function __destruct()
	{
	}

	private function write_log($msg = null, $cmd = false)
	{
		if (!$cmd and $this->cmd != 'debug') {
			file_put_contents(strftime($this->service['log_file']), date('Y-m-d H:i:s') . "\t" . $msg . "\n", FILE_APPEND);
		} else {
			echo $msg, "\n";
		}
	}

	private function update_service_status()
	{
		$this->status = win32_query_service_status($this->service['service']['service']);
	}

	private function write_run($msg = null)
	{
		file_put_contents($this->service['run_file'], date('Y-m-d H:i:s') . "\t" . $msg);
	}

	private function create()
	{
		if (!isset($this->status['CurrentState']) and $this->status == WIN32_ERROR_SERVICE_DOES_NOT_EXIST) {
			$this->write_log('WARNING: Creating service');
			$this->win32_op_service('win32_create_service', $this->service['service'], WIN32_NO_ERROR, 'OK: Service created', true);
		}
	}

	private function delete()
	{
		if (isset($this->status['CurrentState']) and $this->status['CurrentState'] == WIN32_SERVICE_STOPPED) {
			$this->write_log('WARNING: Deleting service');
			$this->win32_op_service('win32_delete_service', $this->service['service']['service'], WIN32_NO_ERROR, 'OK: Service deleted', true);
		}
	}

	private function stop()
	{
		if (isset($this->status['CurrentState']) and $this->status['CurrentState'] == WIN32_SERVICE_RUNNING) {
			$this->write_log('WARNING: Sending stop signal');
			$this->win32_op_service('win32_stop_service', $this->service['service']['service'], WIN32_NO_ERROR, 'OK: Stop signal sent', true);
		}
	}

	private function start()
	{
		if (isset($this->status['CurrentState']) and $this->status['CurrentState'] == WIN32_SERVICE_STOPPED) {
			$this->write_log('WARNING: Sending start signal');
			$this->win32_op_service('win32_start_service', $this->service['service']['service'], WIN32_NO_ERROR, 'OK: Start signal sent', true);
		}
	}

	private function pause()
	{
		if (isset($this->status['CurrentState']) and $this->status['CurrentState'] == WIN32_SERVICE_RUNNING) {
			$this->write_log('WARNING: Sending pause signal');
			$this->win32_op_service('win32_pause_service', $this->service['service']['service'], WIN32_NO_ERROR, 'OK: Pause signal sent', true);
		}
	}

	private function continue()
	{
		if (isset($this->status['CurrentState']) and $this->status['CurrentState'] == WIN32_SERVICE_PAUSED) {
			$this->write_log('WARNING: Sending continue signal');
			$this->win32_op_service('win32_continue_service', $this->service['service']['service'], WIN32_NO_ERROR, 'OK: Continue signal sent', true);
		}
	}

	private function start_service()
	{
		$this->write_log('INFO: Connecting with the service');
		if (!$this->win32_op_service('win32_start_service_ctrl_dispatcher', $this->service['service']['service'])) return(false);
		return $this->set_service_running();
	}

	private function set_service_running()
	{
		$this->write_log('WARNING: Sending running signal');
		return $this->win32_op_service('win32_set_service_status', WIN32_SERVICE_RUNNING, true, 'OK: Service started');
	}

	private function set_service_stopped()
	{
		$this->write_log('WARNING: Sending stopped signal');
		return $this->win32_op_service('win32_set_service_status', WIN32_SERVICE_STOPPED, true, 'OK: Service stopped');
	}

	private function set_service_paused()
	{
		$this->write_log('WARNING: Sending stopped signal');
		return $this->win32_op_service('win32_set_service_status', WIN32_SERVICE_PAUSED, true, 'OK: Service paused');
	}

	private function main_loop($debug = false)
	{
		$this->write_run();
		$this->write_log('Do something', $debug);
	}

	private function win32_op_service($win32_op_service, $param, $cond = true, $msg = null, $debug = false)
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

	private function debug()
	{
		if ((!isset($this->status['CurrentState']) and $this->status == WIN32_ERROR_SERVICE_DOES_NOT_EXIST) or (isset($this->status['CurrentState']) and $this->status['CurrentState'] == WIN32_SERVICE_STOPPED)) {
			$this->write_log('DEBUG Start', true);
			$this->main_loop(true);
			$this->write_log('DEBUG End', true);
		}
	}

	private function run()
	{
		if (isset($this->status['CurrentState']) and $this->status['CurrentState'] == WIN32_SERVICE_START_PENDING) {
			if ($this->start_service()) {
				while (WIN32_SERVICE_CONTROL_STOP != $ctr_msg = win32_get_last_control_message()) {
					if ($ctr_msg === WIN32_SERVICE_CONTROL_INTERROGATE) {
						win32_set_service_status($this->paused ? WIN32_SERVICE_PAUSED : WIN32_SERVICE_RUNNING);

					} elseif ($ctr_msg === WIN32_SERVICE_CONTROL_CONTINUE && $this->status['CurrentState'] == WIN32_SERVICE_PAUSED) {
						$this->write_log('Service Control : Continue');
						$this->paused = false;
						win32_set_service_status(WIN32_SERVICE_CONTINUE_PENDING);
						$this->set_service_running();

					} elseif ($ctr_msg === WIN32_SERVICE_CONTROL_PAUSE && $this->status['CurrentState'] == WIN32_SERVICE_RUNNING) {
						$this->write_log('Service Control : Pause');
						$this->paused = true;
						win32_set_service_status(WIN32_SERVICE_PAUSE_PENDING);
						$this->set_service_paused();

					//} elseif ($ctr_msg === WIN32_SERVICE_CONTROL_PRESHUTDOWN) {
					//} elseif ($ctr_msg === WIN32_SERVICE_CONTROL_SHUTDOWN) {
					//} elseif ($ctr_msg === WIN32_SERVICE_CONTROL_STOP) {
					}

					if (!$this->paused) {
						if ($this->status['CurrentState'] == WIN32_SERVICE_CONTINUE_PENDING){
							$this->set_service_running();
						}
						$this->main_loop();
					}

					if ($this->paused && $this->status['CurrentState'] == WIN32_SERVICE_PAUSE_PENDING){
						$this->set_service_paused();
					}

					$this->write_run('LOOP WAIT');
					sleep($this->service['loop_wait']);
					$this->update_service_status();
				}
				$this->set_service_stopped();
			}
		}
	}
}

$service = [
	'run_file' => __DIR__ . '/service_run.log',
	'log_file' => __DIR__ . '/service_%Y%m%d.log',
	'loop_wait' => 10,
	'service' => [
		'service' => 'WindowsServicePhpTest',
		'display' => 'Windows service PHP test',
		'description' => 'This service is an PHP example for test',
		'path' => '"' . dirname(PHP_BINARY) . '\\php-win.exe"',
		'params' => '"' . __FILE__ . '" run',
		'start_type' => WIN32_SERVICE_AUTO_START,
	],
];

if (!isset($argv[1])) $argv[1] = null;
new WinServiceManager($service, $argv[1]);
?>
