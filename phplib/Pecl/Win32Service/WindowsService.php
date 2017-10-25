<?php

/*
 * Windows Service framework class
 * Author: Credomane
 * Version: 2.0
 *
 * Requirements: php_win32service.dll V1.1-dev
 */

namespace Pecl\Win32Service;

/**
 * Override functions as needed by your application.
 * serviceStart(), serviceTick(), and serviceStop() are designed to be overridden.
 * Call serviceMain() to start your service. Function will not return until windows stops the service.
 * Optionally, override serviceMain() for 100% control.
 */
class WindowsService {

    protected $serviceName = null;
    protected $isValid = false;

    /**
     * Constructor
     * Required Setup
     *
     * @param $serviceName string Required Unique name Windows Service Manager will act upon.
     *
     * @throws \Exception
     */
    public function __construct($serviceName = null) {
        if (!empty($serviceName) and is_string($serviceName)) {
            $this->serviceName = $serviceName;
        } else {
            throw new \Exception('serviceName must be a valid string!');
        }
    }

    /**
     * Initializes the service.
     * Do not call until you are ready.
     * Function will not return until service is stopped.
     */
    public function serviceMain() {
        // Connect to service dispatcher and notify that startup was successful.
        win32_start_service_ctrl_dispatcher($this->serviceName);

        $this->setState(WIN32_SERVICE_START_PENDING);
        $this->serviceStart();
        $this->setState(WIN32_SERVICE_RUNNING);

        while ($this->serviceResponder()) {
            if ($this->getState(WIN32_SERVICE_RUNNING)) {
                $this->serviceTick();
            }
            sleep(1);
        }

        $this->setState(WIN32_SERVICE_STOP_PENDING);
        $this->serviceStop();
        $this->setState(WIN32_SERVICE_STOPPED);
    }

    /**
     * Preforms any setup that is needed by ServiceTick().
     * Should return quickly.
     */
    protected function serviceStart() {

    }

    /**
     * Called repeatedly but no more than once per second.
     * Should return as often as possible or make arrangements
     * to manually call serviceResponder().
     */
    protected function serviceTick() {

    }

    /**
     * Performs any clean up before the service exits.
     */
    protected function serviceStop() {

    }

    /**
     * Called before each serviceTick()
     * Makes sure windows doesn't think we are a zombie.
     */
    protected function serviceResponder() {
        $MESSAGE = win32_get_last_control_message();
        $this->setLastMessage($MESSAGE);
        switch ($MESSAGE) {
            // Returned when our SCM message queue is empty.
            // Which is generally 99% of the time.
            case 0:

                // When SCM wants a status update NOW.
                // Generally 'called' when another process asks SCM about us.
                // Believe this is also 'called' if we don't update our status in SCM regularly.
            case WIN32_SERVICE_CONTROL_INTERROGATE:
                win32_set_service_status($this->getState());
                return true;
                break;

            case WIN32_SERVICE_CONTROL_CONTINUE:
                $this->setState(WIN32_SERVICE_RUNNING);
                return true;
                break;

            case WIN32_SERVICE_CONTROL_PAUSE:
                $this->setState(WIN32_SERVICE_PAUSED);
                return true;
                break;

            case WIN32_SERVICE_CONTROL_PRESHUTDOWN:
            case WIN32_SERVICE_CONTROL_SHUTDOWN:
            case WIN32_SERVICE_CONTROL_STOP:
                return false;
                break;

            default:
                win32_set_service_status(WIN32_ERROR_CALL_NOT_IMPLEMENTED); // Add more cases to handle other service calls
                return true;
        }
    }

    /**
     * Used to set the Service's State.
     */
    private function setState($STATE) {
        $this->STATE = $STATE;
        win32_set_service_status($STATE);
    }

    /**
     * Used to get the Service's State.
     * Passing an parameter will return true/false.
     */
    public function getState($STATE = null) {
        if ($STATE === null) {
            return $this->STATE;
        } else {
            return ($this->STATE == $STATE);
        }
    }


    /**
     * Used to set the last message from SCM.
     */
    private function setLastMessage($MESSAGE) {
        $this->MESSAGE = $MESSAGE;
    }

    /**
     * Used to get the last message from SCM.
     * Passing an parameter will return true/false.
     */
    public function getLastMessage($MESSAGE = null) {
        if ($MESSAGE === null) {
            return $MESSAGE->MESSAGE;
        } else {
            return ($MESSAGE->MESSAGE == $MESSAGE);
        }
    }

}

