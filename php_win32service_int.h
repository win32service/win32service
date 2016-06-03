/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2011 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@php.net>                                    |
  +----------------------------------------------------------------------+
*/

/* $Id: php_win32service_int.h 309260 2011-03-15 17:03:59Z rquadling $ */

#ifndef PHP_WIN32SERVICE_INT_H
#define PHP_WIN32SERVICE_INT_H

#ifdef ZTS
# include "TSRM.h"
# define SVCG(v)	TSRMG(win32service_globals_id, zend_win32service_globals *, v)
#else
# define SVCG(v)	(win32service_globals.v)
#endif

ZEND_BEGIN_MODULE_GLOBALS(win32service)
	HANDLE svc_thread; /* MUST be first in struct */
	DWORD svc_thread_id;
	HANDLE event;
	DWORD  code;
	SERVICE_STATUS 			st;
	SERVICE_STATUS_HANDLE	sh;
	SERVICE_TABLE_ENTRY		te[2];
	char *service_name;
	/* args for the control handler */
	struct {
		DWORD dwControl, dwEventType;
		LPVOID lpEventData;
	} args;
ZEND_END_MODULE_GLOBALS(win32service)

ZEND_DECLARE_MODULE_GLOBALS(win32service);
#endif

