@echo off

cd /d C:\projects\php-src

set PHP_MODULES="php_win32service"

move tests tests_old
move Zend\tests Zend\tests_old
move ext\standard\tests ext\standard\tests_old
move sapi\cgi\tests sapi\cgi\tests_old
move sapi\cli\tests sapi\cli\tests_old
move sapi\fpm\tests sapi\fpm\tests_old
move sapi\phpdbg\tests sapi\phpdbg\tests_old
move sapi\tests sapi\tests_old
move ext\date\tests ext\date\tests_old


setlocal enableextensions enabledelayedexpansion
rem	for %%a in (%ARCHITECTURES%) do (
rem		set ARCH=%%a
		set ARCH=%PLATFORM%
		set SDK_RUNNER=%PHP_BUILD_CACHE_SDK_DIR%\phpsdk-%PHP_BUILD_CRT%-!ARCH!.bat
		if not exist "!SDK_RUNNER!" (
			echo "!SDK_RUNNER!" doesn't exist
			exit /b 3
		)

		cmd /c !SDK_RUNNER! -t %APPVEYOR_BUILD_FOLDER%\appveyor\test_task.bat

		if %errorlevel% neq 0 exit /b 3

rem	)
endlocal

exit /b 0
