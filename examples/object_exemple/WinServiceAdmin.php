<?php
/**
 * This file is part of win32service extension package.
 * This file is an manager for Windows Service.
 *
 * Requirements : PHP 7+
 *
 * php service.php register
 * php service.php delete
 * php service.php debug
 * 
 * This part cannot be delegated to another account 
 * without administrator privileges.
 */

namespace win32service;

include(__DIR__.'/config.php');

class WinServiceAdmin extends WinServiceAbstract {

	protected function allowedCommand() {
		return ['register', 'delete', 'debug'];
	}

	private function register()
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
}


if (!isset($argv[1])) $argv[1] = null;
new WinServiceAdmin($service, $argv[1]);

