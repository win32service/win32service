@echo off

cd /d C:\projects\php-src

set PHP_MODULES=php_win32service

rmdir /Q /S tests

setlocal enableextensions enabledelayedexpansion

set EXTENSIONTOREMOVE=Zend sapi sapi\phpdbg sapi\fpm sapi\cli sapi\cgi ext\standard ext\date ext\spl ext\reflection ext\pcre ext\spl\examples ext\hash ext\json

for %%e in (%EXTENSIONTOREMOVE%) do (
	set EXTNAME=%%e
	if exist "!EXTNAME!\tests" (
		echo Remove tests in !EXTNAME!\tests
		rmdir /Q /S !EXTNAME!\tests
	)
)



rem	for %%a in (%ARCHITECTURES%) do (
rem		set ARCH=%%a
		set ARCH=%PLATFORM%
		set SDK_RUNNER=%PHP_BUILD_CACHE_SDK_DIR%\phpsdk-%PHP_BUILD_CRT%-!ARCH!.bat
		if not exist "!SDK_RUNNER!" (
			echo "!SDK_RUNNER!" doesn't exist
			exit /b 3
		)

		cmd /c !SDK_RUNNER! -t %GITHUB_WORKSPACE%\.github\test_task.bat

		if %errorlevel% neq 0 exit /b 3

rem	)
endlocal

exit /b 0
