[![License](https://img.shields.io/badge/license-PHP_License-blue.svg)](http://www.php.net/license/3_01.txt)
[![Documentation](https://img.shields.io/badge/manual-win32service-blue.svg)](http://php.net/manual/en/book.win32service.php)

# Win32Service
The [win32service](https://pecl.php.net/package/win32service) extension is a Windows specific extension that allows PHP to communicate with the Service Control Manager to start, stop, register and unregister services, and even allows your PHP scripts to run as a service.

# Goal of this repos
The goal is build this extension with php 7.0 in first time. This goal was reached in january 2017.

# Repos organisation
* This [`archived-svn`](https://github.com/InExtenso/win32service/tree/archived-svn) branch is a copy from the [Archived SVN Repository](http://svn.php.net/pecl/win32service/trunk/) with all history of changes. Do not use this but choice another version. 
* The [`master`](https://github.com/InExtenso/win32service/tree/master) branch is the oldest version. Do not use this but choice another version.
* The [`php7`](https://github.com/InExtenso/win32service/tree/php7) branch contains the new version ready for PHP 7.0 and PHP 7.1 and work with all SAPI.
* The [`php7-cli`](https://github.com/InExtenso/win32service/tree/php7-cli) branch contains the original version ready for PHP 7.0 and PHP 7.1 but restricted for work only on CLI-SAPI.
