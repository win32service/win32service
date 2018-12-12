<?php
/**
 * This file is part of win32service extension package.
 * This file is an manager for Windows Service.
 *
 * Requirements : PHP 7+
 */

namespace win32service;

include(__DIR__.'/config.php');

class WinService extends WinServiceAbstract {

	protected function allowedCommand() {
		return ['run'];
	}


	private function update_service_status()
	{
		$this->status = win32_query_service_status($this->service['service']['service']);
	}

	private function write_run($msg = null)
	{
		file_put_contents($this->service['run_file'], date('Y-m-d H:i:s') . "\t" . $msg);
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

//Launch the service with
if (!isset($argv[1])) $argv[1] = null;
new WinService($service, $argv[1]);

