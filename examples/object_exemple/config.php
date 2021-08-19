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
abstract class WinServiceAbstract
{

    /**
     * @var bool
     */
    protected $paused = false;

    /**
     * @var array|null Service configuration array
     */
    protected $service = null;

    /**
     * @var null|array The Win32Service status (from the Windows Service Manager)
     */
    protected $status = null;

    /**
     * @var string|null The command asked
     */
    protected $cmd = null;

    /**
     * @var array|null List of the allowed command
     */
    protected $commands = null;

    /**
     * WinServiceAbstract constructor.
     * @param array $service The service configuration
     * @param string|null $cmd The command will be execute
     * @param bool $debug If true, enable the log output on console
     */
    public function __construct($service, $cmd = null, $debug = false)
    {
        $this->service = $service;
        $this->commands = $this->allowedCommand();
        $this->cmd = $cmd = strtolower($cmd);
        $this->write_log('INFO: Analyzing command', $debug);
        if (empty($cmd) || !in_array($cmd, $this->commands)) {
            $this->write_log('ERROR: Invalid or missing command: ' . var_export($cmd, true), true);
            die (__FILE__ . ' ' . implode('|', $this->commands));
        }
        if (!method_exists($this, $cmd)) {
            $this->write_log('ERROR: Command not implemented: ' . $cmd, true);
            return;
        }
        $this->write_log('INFO: Querying service status', $debug);
        $this->update_service_status();
        $this->write_log('INFO: Executing command: ' . $cmd, $debug);
        $this->$cmd();
    }

    /**
     * @return array
     */
    abstract protected function allowedCommand();

    /**
     * @param string $msg
     * @param bool $stdout
     */
    protected function write_log($msg, $stdout = false)
    {
        if ($stdout === false) {
            file_put_contents(strftime($this->service['log_file']), date('Y-m-d H:i:s') . "\t" . $msg . "\n",
                FILE_APPEND);
            return;
        }
        echo $msg, "\n";
    }

    /**
     * Read the service status from the Windows Service Manager
     */
    private function update_service_status()
    {
        try {
            $this->status = win32_query_service_status($this->service['service']['service']);
        } catch (\Win32ServiceException $e) {
            $this->status = $e->getCode();
        }
    }

    /**
     * @param string $win32_op_service the name of the win32service function will be execute
     * @param mixed $param The param to be passed at the win32service function
     * @param mixed $cond The expected return of the function
     * @param string|null $msg The message written in the log if the function returned value is equal to the $cond parameter
     * @param bool $debug If true the log is sent to the trandard output
     * @return bool
     */
    protected function win32_op_service($win32_op_service, $param, $cond = WIN32_NO_ERROR, $msg = null, $debug = false)
    {
        $err_code = WIN32_NO_ERROR;
        $err_msg = '';
        try {
            $win32_op_service($param);
        } catch (\Win32ServiceException $e) {
            $err_code = $e->getCode();
            $err_msg = $e->getMessage();
        }
        if ($cond === $err_code) {
            if (isset($msg)) {
                $this->write_log($msg, $debug);
            }
            return true;
        } elseif (WIN32_ERROR_ACCESS_DENIED === $err_code) {
            $this->write_log('ERROR: Access denied', $debug);
        } else {
            $this->write_log('ERROR: Win32 Error Code ' . $err_code . ' ' . $err_msg, $debug);
        }
        return false;
    }

    /**
     * Function called in debug mode
     */
    protected function debug()
    {
        if (
            (!isset($this->status['CurrentState']) && $this->status == WIN32_ERROR_SERVICE_DOES_NOT_EXIST) ||
            (isset($this->status['CurrentState']) && $this->status['CurrentState'] == WIN32_SERVICE_STOPPED)
        ) {
            $this->write_log('DEBUG Start', true);
            $this->main_loop(true);
            $this->write_log('DEBUG End', true);
        }
    }
}
