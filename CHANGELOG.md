# Version 0.4.2 (2020-10-29)

* Add contact information for send feedback

# Version 0.4.1 (2019-06-26)

* Remove the PHP version check. For fix the PHP crashes when `phpinfo` is called by CGI SAPI. (issue [#45](https://github.com/win32service/win32service/issues/45))
* Fix PHP 7.4 errors in object example  (issue [#46](https://github.com/win32service/win32service/issues/46))

# Version 0.4.0 (2019-02-20)

* Add `win32_set_service_exit_mode` and `win32_set_service_exit_code` functions
* Add severals constants
* Add `win32_send_custom_control` function
* Add parameters (`recovery`, `dependencies`) for service configuration used with `win32_create_service`.

# Version 0.3.2 (2014-02-14)

* Fixed #42 : Unable to use the parameters if type is not string.

# Version 0.3.1 (2018-12-10)

The reporitory has been transfered to an independent GitHub organisation.
The new website is https://win32service.github.io

* Removing the PHP version check for not working versions
* Add support for PHP 7.3.
* Fix bugs.

# version 0.3.0 (2018-03-09)

* Add support to use a non-admin account for start, stop, pause, resume service. Rights configuration is needed on the target service.
* Add support for PHP 7.2.
* Fix bugs.

# version 0.2.1 (2017-05-02)

* fix issues with PHP 7.1.4

# version 0.2.0 (2017-01-16)

* Enable the extension of all SAPI
* Display the function state of `phpinfo`

# version 0.1.2-RC2 (2017-01-10)

* Upgrade code for work with PHP 7.0.1+ and 7.1.1+
* Add pause and continue function into example script

# version 0.1.2-RC1 (2017-01-03)

* Fix build error and crash
* Add more information into ext section of phpinfo.

# version 0.1.2-dev (2016-10-26)

* Fix build error
* Use AppVeyor for build ext

# version 0.1.0-dev (2011-03-15)

* This version is the historical first version available for PHP 5.3, 5.4, 5.5 and 5.6

