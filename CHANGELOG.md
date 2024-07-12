# Version 1.1.0 beta 1 (2024-07-24)

* Add object \Win32Service\RightInfo
* Add function win32_read_right_access_service()
* Add function win32_read_all_rights_access_service()
* Add function win32_add_right_access_service()
* Add function win32_remove_right_access_service()

# Version 1.0.2 (2022-10-13)

* Rewrite some code (espectaly if/else instruction).
* Add more detail in exception message.
* Fix recovery settings error (issue [#71](https://github.com/win32service/win32service/issues/71))

# Version 1.0.1 (2021-11-30)

* Allow all function reserved for cli SAPI on embed SAPI. (@EionRobb) (issue [#63](https://github.com/win32service/win32service/issues/63))
* Update for PHP 8.1

# Version 1.0.0 (2020-11-30)

This version is only for PHP 8+

* Upgrade to PHP 8.0.
* Remove code for older Windows version (issue [#57](https://github.com/win32service/win32service/issues/57))
* Add `Win32ServiceException` (extends Exception)
* The return value is no longer `false` when call method with wrong parameters
    * win32_start_service_ctrl_dispatcher
    * win32_set_service_exit_mode
    * win32_set_service_exit_code
    * win32_set_service_status
    * win32_create_service
    * win32_delete_service
    * win32_get_last_control_message
    * win32_query_service_status
    * win32_start_service
    * win32_stop_service
    * win32_pause_service
    * win32_continue_service
    * win32_send_custom_control
* Add check in array information provided to `win32_create_service`. On error, this method throw a ValueError.
* For function `win32_send_custom_control`, the PHP error `The control argument value is not between 128 and 255.` is now throw on ValueError.
* For all function needed the service name, a ValueError is throw if the string is empty.
* The PHP Error `This function work only when using the CLI SAPI and called into the service code.` has been converted to `Win32ServiceException` for this method
    * win32_start_service_ctrl_dispatcher
    * win32_set_service_exit_mode
    * win32_set_service_exit_code
    * win32_set_service_status
    * win32_get_last_control_message
* The PHP Warning `Service ctrl dispatcher already running` has been converted to `Win32ServiceException`.
  This exception is throw if function `win32_start_service_ctrl_dispatcher` is called when the control dispatcher is already started.
* The PHP Warning `Failed to start dispatcher thread` has been converted to `Win32ServiceException`.


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

