BUILD THIS EXTENSION
--------------------

# For Production

Please download the pre-build version on [Release page](https://github.com/win32service/win32service/releases).

# For Test and Debug

## Prerequisites

* Install a build environment for PHP 7+. [See for help](https://wiki.php.net/internals/windows/stepbystepbuild)
* Clone this repository on `<path to php source>\..\pecl\win32service`

## Build

Open the `VS2015 Command Line`

### Setup VC environment

* for x86 build `"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall" x86`
* for amd64 build `"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall" amd64`

### Setup PHP environment

Run `<path to php sdk>\php-sdk\bin\phpsdk_setvars.bat`

### Configure build

Run `buildconf.bat` from the PHP source folder. And run the comand for your wanted build 

* ZTS : `configure.bat --disable-all --with-mp=auto --enable-cli --disable-zts --enable-win32service=shared --with-php-build=deps --enable-debug`
* TS : `configure.bat --disable-all --with-mp=auto --enable-cli --enable-zts --enable-win32service=shared --with-php-build=deps --enable-debug`

### Build

Run `nmake` for effective build the PHP core and all extensions enabled.

### Rebuild case

If you want rebuild run `nmake clean && nmake`.

### Install build result

By default, the result of build is installed on this folder `c:\php\debug`. For install run `nmake install` from the PHP source folder.

### Configure PHP

Copy the standard developpement php.ini file into `c:\php\debug`.

Add the extension loading `extension=php_win32service.dll` on the extension section.

Check the php version and loaded extensions with `c:\php\build\php.exe -v` and `c:\php\build\php.exe -m`

You can get the configuration and version of extension with this command `php --ri win32service`.

Expected result :

```
C:\php\debug>php --ri win32service

win32service

Win32 Service support => enabled
Version => 0.1.2-RC1
```

## Enable php.exe Just-in-time (JIT) debug

For enable the jit debug for `php.exe`, open `regedit` and go to this registry key `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\DebugApplications` and add one DWORD value with name `php.exe` and dicimal value `1`.
Now on every crash you can debug PHP.

## Tests

### sample.php

Go to the `win32service` code source and run this command `c:\php\debug\php.exe sample.php install`.
The normal output is `int(0)`.

Now open the windows service manager and search this service `sample dummy PHP service`. This service is stopped.
You can click on "Start" and after every minutes click on "Stop". On the command line run `net start dummyphp` for start and `net stop dummyphp` for stop the service.

After test, you can remove this service with this command `c:\php\debug\php.exe sample.php uninstall`.
The normal output is `int(0)`.

### service_dummy.php

This file control the service `sample dummy PHP service` for start and stop according to their state.

Usage from the `win32service` folder is `c:\php\debug\php.exe service_dummy.php`.

### service.php

This file create, start, stop, delete, debug and run an service named `Windows service PHP test`.

This file can test this functions:

* win32_create_service
* win32_delete_service
* win32_get_last_control_message
* win32_query_service_status
* win32_set_service_status
* win32_start_service_ctrl_dispatcher
* win32_start_service
* win32_stop_service

Usage:

* Create: run `c:\php\debug\php.exe service.php create`
* Start: run `c:\php\debug\php.exe service.php start`
* Stop: run `c:\php\debug\php.exe service.php stop`
* Debug: run `c:\php\debug\php.exe service.php debug`
* Delete: run `c:\php\debug\php.exe service.php delete`

