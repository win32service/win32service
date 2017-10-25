<?php

/*
 * Windows Service Control framework class
 * Author: Credomane
 * Version: 2.0
 *
 * Requirements: php_win32service.dll V1.1-dev
 */

namespace Pecl\Win32Service;

/**
 * There should be no need to override anything here. Functions are provided for everything.
 */
class WindowsServiceControl {
    private $serviceName = null;
    private $dataTable = array(
        "display" => null,
        "description" => null,
        "path" => null,
        "user" => null,
        "password" => null,
        "load_order" => null,
        "svc_type" => null,
        "start_type" => null,
        "error_control" => null,
        "delayed_start" => null,
        "base_priority" => null
    );
    private $parmTable = array(
        "serviceParams" => null,
        "scriptPath" => null,
        "scriptParams" => null
    );


    /**
     * Constructor
     *
     * @param $serviceName string Required Unique name Windows Service Manager will act upon.
     *
     * @throws \Exception
     */
    public function __construct($serviceName = null) {
        if (!empty($serviceName) and is_string($serviceName)) {
            $this->serviceName = $serviceName;
            $this->dataTable["display"] = $serviceName;
            $this->dataTable["path"] = $GLOBALS["_"];
            $this->dataTable["Script"] = $GLOBALS["0"];
        } else {
            throw new \Exception('serviceName must be a valid string!');
        }

    }

    /**
     * Queries SCM about the service and returns an associative array.
     */
    public function queryStatus() {
        return win32_query_service_status($this->serviceName);
    }

    /**
     * Tells SCM to tell the service to stop.
     */
    public function stop() {
        return win32_stop_service($this->serviceName);
    }

    /**
     * Tells SCM to tell the service to start.
     */
    public function start() {
        return win32_start_service($this->serviceName);
    }

    /**
     * Tells SCM to tell the service to pause.
     */
    public function pause() {
        return win32_pause_service($this->serviceName);
    }

    /**
     * Tells SCM to tell the service to continue.
     */
    public function resume() {
        return win32_continue_service($this->serviceName);
    }

    /**
     * Installs the service, if needed.
     * Please see examples for more information on this function.
     * No validation on arguments is performed in this version!
     */
    public function install() {
        $tempArray = array();
        $tempParm = "";

        $tempArray["display"] = $this->serviceName;
        $tempArray["description"] = $this->serviceName;

        foreach ($this->dataTable as $key => $value) {
            if ($value !== null) {
                $tempArray[$key] = $value;
            }
        }

        $tempArray["service"] = $this->serviceName;
        $tempArray["params"] = "";

        if ($this->parmTable["serviceParams"] !== null) {
            $tempArray["params"] .= $this->parmTable["serviceParams"] . " ";
        }

        if ($this->parmTable["scriptPath"] !== null) {
            $tempArray["params"] .= $this->parmTable["scriptPath"] . " ";
        }

        if ($this->parmTable["scriptParams"] !== null) {
            $tempArray["params"] .= $this->parmTable["scriptParams"];
        }

        $x = win32_create_service($tempArray);
        return $x;
    }

    /**
     * Uninstalls the service, if needed.
     */
    public function uninstall() {
        $x = win32_delete_service($this->serviceName);
        return $x;
    }

    /**
     * Changes the user friendly display name in the service list.
     *
     * @param $value string Friendly display name. Passing NULL sets value to serviceName.
     */
    public function setDisplayName($value = null) {
        $this->dataTable["display"] = $value;
    }

    /**
     * Changes the description in the service list.
     *
     * @param $value string Friendly description. Passing NULL sets value to serviceName.
     */
    public function setDescription($value = null) {
        $this->dataTable["description"] = $value;
    }

    /**
     * Changes the user the service will run as.
     *
     * @param $user     string username to login as. Passing NULL runs service as LocalSystem.
     * @param $password string password to user account. Passing NULL assigns no password.
     */
    public function setLogin($user = null, $password = null) {
        $this->dataTable["user"] = $user;
        $this->dataTable["password"] = ($user === null) ? null : $password;
    }

    /**
     * Sets the executable SCM will start for the service.
     *
     * @NOTES Not passing a parameter to the function will treat it as FALSE.
     *        FALSE will cause no changes.
     *        TRUE will reset the path to the default.
     *        Any other value will change the path.
     *
     * @param $value string|boolean Executable for the service. Should be an absolute path. Default = "Current php executable".
     */
    public function setPath($value = false) {
        if ($value === false) {
            //We do nothing on FALSE
        } elseif ($value === true or empty($value)) {
            $this->dataTable["path"] = '"' . $GLOBALS["_"] . '"';
        } else {
            $this->dataTable["path"] = '"' . $value . '"';
        }
    }

    /**
     * Sets the parameters SCM starts the service with.
     * @NOTES Not passing one of the parameters to the function will treat it as FALSE.
     *        FALSE will cause no change.
     *        TRUE will reset the parameter to the default.
     *        NULL will clear parameter.
     *        Any other value will change the stored parameter.
     *
     * @param $serviceParams boolean|string Optional, Arguments to pass to executable. Default = null
     * @param $scriptPath    boolean|string Optional, Path to php script Should be an absolute path. Default = script php launched with.
     * @param $scriptParams  boolean|string Optional, Arguments to pass to the php script. Default = null
     */
    public function setParams($serviceParams = false, $scriptPath = false, $scriptParams = false) {
        if ($serviceParams === true) {
            $this->parmTable["serviceParams"] = null;
        } elseif ($scriptPath === null) {
            $this->parmTable["serviceParams"] = null;
        } elseif ($serviceParams === false) {
            //We do nothing on FALSE
        } else {
            $this->parmTable["serviceParams"] = $serviceParams;
        }

        if ($scriptPath !== true) {
            $this->parmTable["scriptPath"] = '"' . $GLOBALS["0"] . '"';
        } elseif ($scriptPath === null) {
            $this->parmTable["scriptPath"] = null;
        } elseif ($scriptPath === false) {
            //We do nothing on FALSE
        } else {
            $this->parmTable["scriptPath"] = '"' . $scriptPath . '"';
        }

        if ($scriptParams !== true) {
            $this->parmTable["scriptParams"] = null;
        } elseif ($scriptParams === null) {
            $this->parmTable["scriptPath"] = null;
        } elseif ($scriptParams === false) {
            //We do nothing on FALSE|NULL
        } else {
            $this->parmTable["scriptParams"] = $scriptParams;
        }
    }

    /**
     * Sets the order group in which SCM starts the service.
     *
     * @param $value string See Windows docs on function CreateService parameter lpLoadOrderGroup. Default = null
     */
    public function setLoadOrder($value = null) {
        $this->dataTable["load_order"] = $value;
    }

    /**
     * Sets the service type and changes how SCM treats it.
     *
     * @param $value integer See "dwServiceType" constants for valid values. Default = null. Which makes it WIN32_SERVICE_WIN32_OWN_PROCESS.
     */
    public function setServiceType($value = null) {
        $this->dataTable["svc_type"] = $value;
    }

    /**
     * Changes how SCM starts the service.
     *
     * @param $value integer See "dwStartType" constants for valid values. Default = null. Which makes it WIN32_SERVICE_AUTO_START.
     */
    public function setStartType($value = null) {
        $this->dataTable["start_type"] = $value;
    }

    /**
     * How will SCM handle errors from the service during start up?
     *
     * @param $value integer See "dwErrorControl" constants for valid values. Default = WIN32_SERVICE_ERROR_IGNORE.
     */
    public function setErrorControl($value = null) {
        $this->dataTable["error_control"] = $value;
    }

    /**
     * Allows Vista and newer to delay start up of the service to speed up boot times.
     *
     * @param $value boolean Delay startup? Default = false.
     */
    public function setDelayedStart($value = false) {
        if (gettype($value) != "BOOLEAN") {
            $value = false;
        }
        $this->dataTable["delayed_start"] = $value;
    }

    /**
     * Changes the base priority windows will give the service.
     *
     * @param $value integer See "Win32 Priority Constants". Default = WIN32_NORMAL_PRIORITY_CLASS.
     */
    public function setPiority($value = null) {
        $this->dataTable["base_priority"] = $value;
    }


}

