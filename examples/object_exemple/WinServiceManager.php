<?php
/**
 * This file is part of win32service extension package.
 * This file is an manager for Windows Service.
 *
 * Requirements : PHP 7+
 *
 * php service.php start
 * php service.php stop
 * php service.php pause
 * php service.php continue
 * php service.php debug
 * 
 * This part can be delegated to another account without 
 * administrator privileges.
 */

namespace win32service;

include(__DIR__.'/config.php');

class WinServiceManager extends WinServiceAbstract {

	protected function allowedCommand() {
		return ['stop', 'start', 'pause', 'continue', 'debug'];
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
}



if (!isset($argv[1])) $argv[1] = null;
new WinServiceManager($service, $argv[1]);

