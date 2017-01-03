BUILD THIS EXTENSION
--------------------

# For Production

Please download the pre-build version on [Release page](https://github.com/InExtenso/win32service/releases).

# For Test and Debug

## Prerequisites

* Install an build environment for PHP 7+. [See for help](https://wiki.php.net/internals/windows/stepbystepbuild)
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

* ZTS : `configure.bat --with-mp=auto --enable-cli --disable-zts --enable-win32service=shared --with-php-build=deps --enable-debug --enable-mbstring`
* TS : `configure.bat --with-mp=auto --enable-cli --enable-zts --enable-win32service=shared --with-php-build=deps --enable-debug --enable-mbstring`

### Build

Run `nmake` for effective build the PHP core and all extention enabled.

### Rebuild case

If you want rebuild run `nmake clean && nmake`.

### Install build result

By default, the result of build is installed on this folder `c:\php\debug`. For install run `nmake install` from the PHP source folder.


## Enable php.exe Just-in-time debug

For enable the jit debug for `php.exe`, open `regedit` and go to this registry key `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\DebugApplications` and add one DWORD value with name `php.exe` and dicimal value `1`.
Now on every crash you can debug PHP.

## Tests

Go to the `win32service` code source and run this command `c:\php\debug\php.exe sample.php install`.
The normal output is `int(0)`.

Now open the windows service manager and search this service `sample dummy PHP service`. This service is stopped.
You can click on "Start" and after every minutes click on "Stop". On the command line run `net start dummyphp` for start and `net stop dummyphp` for stop the service.

After test, you can remove this service with this command `c:\php\debug\php.exe sample.php uninstall`.
The normal output is `int(0)`.

