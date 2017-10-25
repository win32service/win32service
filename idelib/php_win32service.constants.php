<?php


/*
 * php_win32service.dll provided constants
 * Author: Credomane
 * Date: 5/25/16
 * Version: 2.2
 *
 * Requirements: php_win32service.dll V1.6-dev
 */


/*
 * This file is NOT to be used in production!
 * This file is for IDE's so they don't complain about unknown constants.
 * Also allows IDE's to know how to use them. Made for Eclipse + PDT
 * Your mileage may vary. 
 */


/*
 * WARNING: Commented out defines are NOT supported. Meaning they literally will not work in most cases.
 * See the Microsoft documentation for better details. Just remove the "WIN32_" when searching. :)
 */

/* Constants used in communication with the SCM */


/**
 * dwServiceType
 *
 * Type of services
 */
define("WIN32_SERVICE_KERNEL_DRIVER",                   0x00000001, true);  /* 0x00000001 Driver service. */
define("WIN32_SERVICE_FILE_SYSTEM_DRIVER",              0x00000002, true);  /* 0x00000002 File system driver service. */
define("WIN32_SERVICE_ADAPTER",                         0x00000004, true);  /* 0x00000004 Reserved. */
define("WIN32_SERVICE_RECOGNIZER_DRIVER",               0x00000008, true);  /* 0x00000008 Reserved. */
define("WIN32_SERVICE_WIN32_OWN_PROCESS",               0x00000010, true);  /* 0x00000010 Service that runs in its own process. */
define("WIN32_SERVICE_WIN32_SHARE_PROCESS",             0x00000020, true);  /* 0x00000020 Service that shares a process with one or more other services. */
define("WIN32_SERVICE_INTERACTIVE_PROCESS",             0x00000100, true);  /* 0x00000100 The service can interact with the desktop. */
define("WIN32_SERVICE_WIN32_OWN_PROCESS_INTERACTIVE",   0x00000110, true);  /* 0x00000110 Service that runs in its own process and interacts with the desktop. */




/* dwCurrentState */
define("WIN32_SERVICE_STOPPED",             0x0000001, true);   /* 0x00000001 The service is not running. */
define("WIN32_SERVICE_START_PENDING",       0x0000002, true);   /* 0x00000002 The service is starting. */
define("WIN32_SERVICE_STOP_PENDING",        0x0000003, true);   /* 0x00000003 The service is stopping. */
define("WIN32_SERVICE_RUNNING",             0x0000004, true);   /* 0x00000004 The service is running. */
define("WIN32_SERVICE_CONTINUE_PENDING",    0x0000005, true);   /* 0x00000005 The service continue is pending. */
define("WIN32_SERVICE_PAUSE_PENDING",       0x0000006, true);   /* 0x00000006 The service pause is pending. */
define("WIN32_SERVICE_PAUSED",              0x0000007, true);   /* 0x00000007 The service is paused. */




/* dwControl */
define("WIN32_SERVICE_CONTROL_CONTINUE",                0x00000003, true);  /* 0x00000003 Notifies a paused service that it should resume. */
define("WIN32_SERVICE_CONTROL_DEVICEEVENT",             0x0000000B, true);  /* 0x0000000B */
define("WIN32_SERVICE_CONTROL_HARDWAREPROFILECHANGE",   0x0000000C, true);  /* 0x0000000C */
define("WIN32_SERVICE_CONTROL_INTERROGATE",             0x00000004, true);  /* 0x00000004 Notifies a service that it should report its current status information to the service control manager. */
define("WIN32_SERVICE_CONTROL_NETBINDADD",              0x00000007, true);  /* 0x00000007 Notifies a network service that there is a new component for binding. */
define("WIN32_SERVICE_CONTROL_NETBINDDISABLE",          0x0000000A, true);  /* 0x0000000A Notifies a network service that one of its bindings has been disabled. */
define("WIN32_SERVICE_CONTROL_NETBINDENABLE",           0x00000009, true);  /* 0x00000009 Notifies a network service that a disabled binding has been enabled. */
define("WIN32_SERVICE_CONTROL_NETBINDREMOVE",           0x00000008, true);  /* 0x00000008 Notifies a network service that a component for binding has been removed. */
define("WIN32_SERVICE_CONTROL_PARAMCHANGE",             0x00000006, true);  /* 0x00000006 Notifies a service that its startup parameters have changed. */
define("WIN32_SERVICE_CONTROL_PAUSE",                   0x00000002, true);  /* 0x00000002 Notifies a service that it should pause. */
define("WIN32_SERVICE_CONTROL_POWEREVENT",              0x0000000D, true);  /* 0x0000000D */
define("WIN32_SERVICE_CONTROL_PRESHUTDOWN",             0x0000000F, true);  /* 0x0000000F Notifies a service that the system will be shutting down.
                                                                                          Services that need additional time to perform cleanup tasks beyond the tight time restriction at system shutdown can use this notification.
                                                                                          The service control manager sends this notification to applications that have registered for it before sending a SERVICE_CONTROL_SHUTDOWN notification to applications that have registered for that notification.
                                                                                          A service that handles this notification blocks system shutdown until the service stops or the preshutdown time-out interval specified through SERVICE_PRESHUTDOWN_INFO expires.
                                                                                          Because this affects the user experience, services should use this feature only if it is absolutely necessary to avoid data loss or significant recovery time at the next system start.
                                                                                          Windows Server 2003 and Windows XP/2000: This value is not supported. */
define("WIN32_SERVICE_CONTROL_SESSIONCHANGE",           0x0000000E, true);  /* 0x0000000E */
define("WIN32_SERVICE_CONTROL_SHUTDOWN",                0x00000005, true);  /* 0x00000005 Notifies a service that the system is shutting down so the service can perform cleanup tasks.
                                                                                          Note that services that register for SERVICE_CONTROL_PRESHUTDOWN notifications cannot receive this notification because they have already stopped.
                                                                                          If a service accepts this control code, it must stop after it performs its cleanup tasks and return NO_ERROR.
                                                                                          After the SCM sends this control code, it will not send other control codes to the service. */
define("WIN32_SERVICE_CONTROL_STOP",                    0x00000001, true);  /* 0x00000001 Notifies a service that it should stop. */




/* dwControlsAccepted */
define("WIN32_SERVICE_ACCEPT_HARDWAREPROFILECHANGE",    0x00000020, true);  /* 0x00000020 The service is notified when the computer's hardware profile has changed.
                                                                                          This enables the system to send SERVICE_CONTROL_HARDWAREPROFILECHANGE notifications to the service. */
define("WIN32_SERVICE_ACCEPT_NETBINDCHANGE",            0x00000010, true);  /* 0x00000010 The service is a network component that can accept changes in its binding without being stopped and restarted.
                                                                                          This control code allows the service to receive SERVICE_CONTROL_NETBINDADD, SERVICE_CONTROL_NETBINDREMOVE, SERVICE_CONTROL_NETBINDENABLE, and SERVICE_CONTROL_NETBINDDISABLE notifications. */
define("WIN32_SERVICE_ACCEPT_PARAMCHANGE",              0x00000008, true);  /* 0x00000008 The service can reread its startup parameters without being stopped and restarted.
                                                                                          This control code allows the service to receive SERVICE_CONTROL_PARAMCHANGE notifications. */
define("WIN32_SERVICE_ACCEPT_PAUSE_CONTINUE",           0x00000002, true);  /* 0x00000002 The service can be paused and continued.
                                                                                          This control code allows the service to receive SERVICE_CONTROL_PAUSE and SERVICE_CONTROL_CONTINUE notifications. */
define("WIN32_SERVICE_ACCEPT_POWEREVENT",               0x00000040, true);  /* 0x00000040 The service is notified when the computer's power status has changed.
                                                                                          This enables the system to send SERVICE_CONTROL_POWEREVENT notifications to the service. */
define("WIN32_SERVICE_ACCEPT_PRESHUTDOWN",              0x00000100, true);  /* 0x00000100 The service can perform preshutdown tasks.
                                                                                          This control code enables the service to receive SERVICE_CONTROL_PRESHUTDOWN notifications.
                                                                                          Note that ControlService and ControlServiceEx cannot send this notification; only the system can send it.
                                                                                          Windows Server 2003 and Windows XP/2000: This value is not supported. */
define("WIN32_SERVICE_ACCEPT_SESSIONCHANGE",            0x00000080, true);  /* 0x00000080 The service is notified when the computer's session status has changed.
                                                                                          This enables the system to send SERVICE_CONTROL_SESSIONCHANGE notifications to the service.
                                                                                          Windows 2000: This value is not supported. */
define("WIN32_SERVICE_ACCEPT_SHUTDOWN",                 0x00000004, true);  /* 0x00000004 The service is notified when system shutdown occurs.
                                                                                          This control code allows the service to receive SERVICE_CONTROL_SHUTDOWN notifications.
                                                                                          Note that ControlService and ControlServiceEx cannot send this notification; only the system can send it. */
define("WIN32_SERVICE_ACCEPT_STOP",                     0x00000001, true);  /* 0x00000001 The service can be stopped.
                                                                                          This control code allows the service to receive SERVICE_CONTROL_STOP notifications. */
define("WIN32_SERVICE_ACCEPT_TIMECHANGE",               0x00000200, true);  /* 0x00000200 The service is notified when the system time has changed.
                                                                                          This enables the system to send SERVICE_CONTROL_TIMECHANGE notifications to the service.
                                                                                          Windows Server 2008, Windows Vista, Windows Server 2003, and Windows XP/2000: This control code is not supported. */
define("WIN32_SERVICE_ACCEPT_TRIGGEREVENT",             0x00000400, true);  /* 0x00000400 The service is notified when an event for which the service has registered occurs.
                                                                                          This enables the system to send SERVICE_CONTROL_TRIGGEREVENT notifications to the service.
                                                                                          Windows Server 2008, Windows Vista, Windows Server 2003, and Windows XP/2000: This control code is not supported. */




/* dwStartType */
define("WIN32_SERVICE_BOOT_START",      0x00000000, true);  /* 0x00000000 A device driver started by the system loader. This value is valid only for driver services. */
define("WIN32_SERVICE_SYSTEM_START",    0x00000001, true);  /* 0x00000001 A device driver started by the IoInitSystem function. This value is valid only for driver services. */
define("WIN32_SERVICE_AUTO_START",      0x00000002, true);  /* 0x00000002 A service started automatically by the service control manager during system startup. */
define("WIN32_SERVICE_DEMAND_START",    0x00000003, true);  /* 0x00000003 A service started by the service control manager when a process calls the StartService function. */
define("WIN32_SERVICE_DISABLED",        0x00000004, true);  /* 0x00000004 A service that cannot be started. Attempts to start the service result in the error code ERROR_SERVICE_DISABLED. */




/* dwErrorControl */
define("WIN32_SERVICE_ERROR_IGNORE",    0x00000000, true);  /* 0x00000000 The startup program ignores the error and continues the startup operation. */
define("WIN32_SERVICE_ERROR_NORMAL",    0x00000001, true);  /* 0x00000001 The startup program logs the error in the event log but continues the startup operation. */
define("WIN32_SERVICE_ERROR_SEVERE",    0x00000002, true);  /* 0x00000002 The startup program logs the error in the event log.
                                                                          If the last-known-good configuration is being started, the startup operation continues. Otherwise, the system is restarted with the last-known-good configuration. */
define("WIN32_SERVICE_ERROR_CRITICAL",  0x00000003, true);  /* 0x00000003 The startup program logs the error in the event log, if possible.
                                                                          If the last-known-good configuration is being started, the startup operation fails. Otherwise, the system is restarted with the last-known good configuration. */




/* dwServiceFlags */
define("WIN32_SERVICE_RUNS_IN_SYSTEM_PROCESS",  0x00000001, true);  /* 0x00000001 The service runs in a system process that must always be running. */




/* Error constants generated in communication with the SCM */
define("WIN32_ERROR_ACCESS_DENIED",                     0x00000005, true);  /* 0x00000005 The handle to the SCM database does not have the appropriate access rights. */
define("WIN32_ERROR_CALL_NOT_IMPLEMENTED",              0x00000078, true);  /* 0x00000028 This function is not supported. */
define("WIN32_ERROR_CIRCULAR_DEPENDENCY",               0x00000423, true);  /* 0x00000423 A circular service dependency was specified. */
define("WIN32_ERROR_DATABASE_DOES_NOT_EXIST",           0x00000429, true);  /* 0x00000429 The specified database does not exist. */
define("WIN32_ERROR_DEPENDENT_SERVICES_RUNNING",        0x0000041B, true);  /* 0x0000041B The service cannot be stopped because other running services are dependent on it. */
define("WIN32_ERROR_DUPLICATE_SERVICE_NAME",            0x00000436, true);  /* 0x00000436 The display name already exists in the service control manager database either as a service name or as another display name. */
define("WIN32_ERROR_FAILED_SERVICE_CONTROLLER_CONNECT", 0x00000427, true);  /* 0x00000427 The service process could not connect to the service controller. */
define("WIN32_ERROR_INSUFFICIENT_BUFFER",               0x0000007A, true);  /* 0x0000007A The buffer is too small for the SERVICE_STATUS_PROCESS structure. Nothing was written to the structure. */
define("WIN32_ERROR_INVALID_DATA",                      0x0000000D, true);  /* 0x0000000D The specified service status structure is invalid. */
define("WIN32_ERROR_INVALID_HANDLE",                    0x00000006, true);  /* 0x00000006 The handle to the specified service control manager database is invalid. */
define("WIN32_ERROR_INVALID_LEVEL",                     0x0000007C, true);  /* 0x0000007C The InfoLevel parameter contains an unsupported value. */
define("WIN32_ERROR_INVALID_NAME",                      0x0000007B, true);  /* 0x0000007B The specified service name is invalid. */
define("WIN32_ERROR_INVALID_PARAMETER",                 0x00000057, true);  /* 0x00000057 A parameter that was specified is invalid (CreateService) or the cbSize member of SERVICE_STATUS_PROCESS is not valid (QueryServiceStatusEx). */
define("WIN32_ERROR_INVALID_SERVICE_ACCOUNT",           0x00000421, true);  /* 0x00000421 The user account name specified in the lpServiceStartName parameter does not exist. */
define("WIN32_ERROR_INVALID_SERVICE_CONTROL",           0x0000041C, true);  /* 0x0000041C The requested control code is not valid, or it is unacceptable to the service. */
define("WIN32_ERROR_PATH_NOT_FOUND",                    0x00000003, true);  /* 0x00000003 The service binary file could not be found. */
define("WIN32_ERROR_SERVICE_ALREADY_RUNNING",           0x00000420, true);  /* 0x00000420 An instance of the service is already running. */
define("WIN32_ERROR_SERVICE_CANNOT_ACCEPT_CTRL",        0x00000425, true);  /* 0x00000425 The requested control code cannot be sent to the service because the state of the service is SERVICE_STOPPED, SERVICE_START_PENDING, or SERVICE_STOP_PENDING. */
define("WIN32_ERROR_SERVICE_DATABASE_LOCKED",           0x0000041F, true);  /* 0x0000041F The database is locked. */
define("WIN32_ERROR_SERVICE_DEPENDENCY_DELETED",        0x00000433, true);  /* 0x00000433 The service depends on a service that does not exist or has been marked for deletion. */
define("WIN32_ERROR_SERVICE_DEPENDENCY_FAIL",           0x0000042C, true);  /* 0x0000042C The service depends on another service that has failed to start. */
define("WIN32_ERROR_SERVICE_DISABLED",                  0x00000422, true);  /* 0x00000422 The service has been disabled. */
define("WIN32_ERROR_SERVICE_DOES_NOT_EXIST",            0x00000424, true);  /* 0x00000424 The specified service does not exist as an installed service. */
define("WIN32_ERROR_SERVICE_EXISTS",                    0x00000431, true);  /* 0x00000431 The specified service already exists in this database. */
define("WIN32_ERROR_SERVICE_LOGON_FAILED",              0x0000042D, true);  /* 0x0000042D The service did not start due to a logon failure. This error occurs if the service is configured to run under an account that does not have the "Log on as a service" right. */
define("WIN32_ERROR_SERVICE_MARKED_FOR_DELETE",         0x00000430, true);  /* 0x00000430 The specified service has already been marked for deletion. */
define("WIN32_ERROR_SERVICE_NO_THREAD",                 0x0000041E, true);  /* 0x0000041E A thread could not be created for the service. */
define("WIN32_ERROR_SERVICE_NOT_ACTIVE",                0x00000426, true);  /* 0x00000426 The service has not been started. */
define("WIN32_ERROR_SERVICE_REQUEST_TIMEOUT",           0x0000041D, true);  /* 0x0000041D The process for the service was started, but it did not call StartServiceCtrlDispatcher, or the thread that called StartServiceCtrlDispatcher may be blocked in a control handler function. */
define("WIN32_ERROR_SHUTDOWN_IN_PROGRESS",              0x0000045B, true);  /* 0x0000045B The system is shutting down; this function cannot be called. */
define("WIN32_NO_ERROR",                                0x00000000, true);  /* 0x00000000 No error. */




/* Win32 Priority Constants */
define("WIN32_ABOVE_NORMAL_PRIORITY_CLASS", 0x00008000, true);  /* 0x00008000 Process that has priority above NORMAL_PRIORITY_CLASS but below HIGH_PRIORITY_CLASS. */
define("WIN32_BELOW_NORMAL_PRIORITY_CLASS", 0x00004000, true);  /* 0x00004000 Process that has priority above IDLE_PRIORITY_CLASS but below NORMAL_PRIORITY_CLASS. */
define("WIN32_HIGH_PRIORITY_CLASS",         0x00000080, true);  /* 0x00000080 Process that performs time-critical tasks that must be executed immediately. The threads of the process preempt the threads of normal or idle priority class processes. An example is the Task List, which must respond quickly when called by the user, regardless of the load on the operating system. Use extreme care when using the high-priority class, because a high-priority class application can use nearly all available CPU time. */
define("WIN32_IDLE_PRIORITY_CLASS",         0x00000040, true);  /* 0x00000040 Process whose threads run only when the system is idle. The threads of the process are preempted by the threads of any process running in a higher priority class. An example is a screen saver. The idle-priority class is inherited by child processes. */
define("WIN32_NORMAL_PRIORITY_CLASS",       0x00000020, true);  /* 0x00000020 Process with no special scheduling needs. */
define("WIN32_REALTIME_PRIORITY_CLASS",     0x00000100, true);  /* 0x00000100 Process that has the highest possible priority. The threads of the process preempt the threads of all other processes, including operating system processes performing important tasks. For example, a real-time process that executes for more than a very brief interval can cause disk caches not to flush or cause the mouse to be unresponsive. */

