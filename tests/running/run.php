<?php

$flog = fopen(__DIR__ . '/run.log', 'w+');
try {
    $serviceName = 'WindowsServicePhpTestRun';

    $currentStats = win32_query_service_status($serviceName);
    fwrite($flog, 'pause/resume old state '.(win32_set_service_pause_resume_state(false) ? 'yes':'no')."\n");
    fwrite($flog, 'pause/resume old state '.(win32_set_service_pause_resume_state(true) ? 'yes':'no')."\n");

    fwrite($flog, 'exit code old '.win32_set_service_exit_code(42)."\n");
    fwrite($flog, 'exit code read '.win32_set_service_exit_code()."\n");
    fwrite($flog, 'exit code read again '.win32_set_service_exit_code()."\n");

    win32_set_service_pause_resume_state(false);
    win32_start_service_ctrl_dispatcher($serviceName);

    try {
        win32_start_service_ctrl_dispatcher($serviceName);
    } catch (Throwable $e) {
        if ($flog) {
            fwrite($flog, sprintf("%s: (%d) %s\n", get_class($e), $e->getCode(), $e->getMessage()));
        }
    }

    try {
        win32_set_service_pause_resume_state(true);
    } catch (Throwable $e) {
        if ($flog) {
            fwrite($flog, sprintf("%s: (%d) %s\n", get_class($e), $e->getCode(), $e->getMessage()));
        }
    }


    $paused = false;

    if (isset($currentStats['CurrentState']) and $currentStats['CurrentState'] == WIN32_SERVICE_START_PENDING) {
        win32_set_service_status(WIN32_SERVICE_RUNNING);
        while (WIN32_SERVICE_CONTROL_STOP != $ctr_msg = win32_get_last_control_message()) {
            if ($ctr_msg === WIN32_SERVICE_CONTROL_INTERROGATE) {
                win32_set_service_status($paused ? WIN32_SERVICE_PAUSED : WIN32_SERVICE_RUNNING);
            }
            sleep(10);
            $currentStats = win32_query_service_status($serviceName);
        }
        win32_set_service_status(WIN32_SERVICE_STOPPED);
    }
} catch (Throwable $e) {
    if ($flog) {
        fwrite($flog,
            sprintf("%s: (%d) %s in %s on line %d\n", get_class($e), $e->getCode(), $e->getMessage(), $e->getFile(),
                $e->getLine()));
    }
}
if ($flog) {
    fclose($flog);
}
