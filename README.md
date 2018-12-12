[![Build status](https://ci.appveyor.com/api/projects/status/7wqljie1knsrtfkh/branch/less_admin?svg=true)](https://ci.appveyor.com/project/macintoshplus/win32service/branch/less_admin)
[![License](https://img.shields.io/badge/license-PHP_License-blue.svg)](http://www.php.net/license/3_01.txt)
[![Documentation](https://img.shields.io/badge/manual-win32service-blue.svg)](http://php.net/manual/en/book.win32service.php)

# Win32Service

The [win32service](https://pecl.php.net/package/win32service) extension is a Windows-specific extension that allows PHP to communicate with the Service Control Manager to start, stop, register and unregister the services, and even allows your PHP scripts to run as a service.

# PHP 7+

This version is available for PHP 7.0.1+, PHP 7.1.1+, PHP 7.2, PHP 7.3 and work for all SAPI.
The PHP version 7.0.0 and 7.1.0 don’t work with this extension use the latest version of PHP 7.0 and PHP 7.1.

Read the `phpinfo()` to know the available function on the current SAPI.

__Caution : PHP 7.0 is the end of life in December 2018__ See : http://php.net/supported-versions.php

# Less administrator privileges

From the version 0.3.0, this extension allows control the service at a low level privileges account.

* Create and add the service
* Set minimal ACL `RPWPRC` onto the service of `sc set`. Note this command overwrite all ACL. Read first the current ACL with `sc show`. See https://support.microsoft.com/en-us/help/914392/best-practices-and-guidance-for-writers-of-service-discretionary-access-control-lists
* Enable the win32service extension build from this branch into your PHP installation.
* Use the account with low-level privileges for running the PHP script to control the defined service. All work.
Now attempt control another service without set the ACL. Nothing work.


# Install

[Download the latest](https://github.com/win32service/win32service/releases) version for your PHP 7 version.

Unzip the package and copy the extension into the folder `ext` of your PHP installation.

After copies, edit the file `php.ini` and add one line to load the extension depending on your version of PHP (7.0/7.1/7.2/7.3, x64/x86, ZTS/NTS). In the example for PHP 7.3 in TS mode and 32-bit architecture : `extension = php_win32service-7.3-ts-vc14-x86.dll` (or use the short name available since PHP 7.2 : `extension = win32service` if the extension file is named `php_win32service.dll`).

Check in command line if the extension is correctly loaded with this command `php --ri win32service`.

If the output indicate this extension is not loaded, check the PHP configuration.

# Use

You can use this [PHP service-library](https://github.com/win32service/service-library) for implements your service runtime and management scripts.

Otherwise , the downloaded zip contains examples scripts used for testing. The file `service.php` contains a skeleton for implement one service in PHP.

To use this example, open a command line window in administrator mode. Go to the example folder and run this command `php service.php create` to installing the test service.

Now a new service is added into the Windows service manager. You can start, stop this service from the Windows service manager or from the administrator command line.

Good coding !

# Help

Crash, feature requests, or suggest, please open an issue.

Help for the extension function, visit the [official PHP web site](http://php.net/manual/en/book.win32service.php)

[Extension PHP pour réaliser des services Windows [FR]](https://nahan.fr/extension-php-pour-realiser-des-services-windows/)

# Contributing

If you want to contribute, you can open an issue to propose your idea. If you can write the code, fork this repository and write the code, finally propose your enhancement by a pull request.
