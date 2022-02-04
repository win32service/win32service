@echo off
setlocal enableextensions enabledelayedexpansion

	cd /D %GITHUB_WORKSPACE%
	if %errorlevel% neq 0 exit /b 3

	set STABILITY=staging
	set DEPS_DIR=%PHP_BUILD_CACHE_BASE_DIR%\deps-%PHP_REL%-%PHP_SDK_VC%-%PHP_SDK_ARCH%
	rem SDK is cached, deps info is cached as well
	echo Updating dependencies in %DEPS_DIR%
	cmd /c phpsdk_deps --update --no-backup --branch %PHP_REL% --stability %STABILITY% --deps %DEPS_DIR% --crt %PHP_BUILD_CRT%
	if %errorlevel% neq 0 exit /b 3

	rem Something went wrong, most likely when concurrent builds were to fetch deps
	rem updates. It might be, that some locking mechanism is needed.
	if not exist "%DEPS_DIR%" (
		cmd /c phpsdk_deps --update --force --no-backup --branch %PHP_REL% --stability %STABILITY% --deps %DEPS_DIR% --crt %PHP_BUILD_CRT%
	)
	if %errorlevel% neq 0 exit /b 3



	for %%z in (%ZTS_STATES%) do (
		set ZTS_STATE=%%z
		if "!ZTS_STATE!"=="enable" set ZTS_SHORT=ts
		if "!ZTS_STATE!"=="disable" set ZTS_SHORT=nts

		cd /d C:\projects\php-src

		cmd /c buildconf.bat --force

		if %errorlevel% neq 0 exit /b 3

        set CFLAGS=/W1 /WX

		cmd /c configure.bat --disable-all --with-mp=auto --enable-cli --!ZTS_STATE!-zts --enable-win32service=shared --enable-object-out-dir=%PHP_BUILD_OBJ_DIR% --with-config-file-scan-dir=%GITHUB_WORKSPACE%\build\modules.d --with-prefix=%GITHUB_WORKSPACE%\build --with-php-build=%DEPS_DIR%

		if %errorlevel% neq 0 exit /b 3

		nmake /NOLOGO

		if %errorlevel% neq 0 exit /b 3

		nmake install

		if %errorlevel% neq 0 exit /b 3

		cd /d %GITHUB_WORKSPACE%

		if not exist "%GITHUB_WORKSPACE%\build\ext\php_win32service.dll" exit /b 3
		if not exist "%GITHUB_WORKSPACE%\build\modules.d" mkdir "%GITHUB_WORKSPACE%\build\modules.d"

		echo extension=php_win32service.dll > %GITHUB_WORKSPACE%\build\modules.d\win32service.ini

		dir %GITHUB_WORKSPACE%\build\modules.d

		xcopy %GITHUB_WORKSPACE%\LICENSE %GITHUB_WORKSPACE%\php_win32service-%PHP_REL%-!ZTS_SHORT!-%PHP_BUILD_CRT%-%PHP_SDK_ARCH%\ /y /f
		xcopy %GITHUB_WORKSPACE%\examples %GITHUB_WORKSPACE%\php_win32service-%PHP_REL%-!ZTS_SHORT!-%PHP_BUILD_CRT%-%PHP_SDK_ARCH%\examples\ /y /f
		xcopy %GITHUB_WORKSPACE%\build\ext\php_win32service.dll %GITHUB_WORKSPACE%\php_win32service-%PHP_REL%-!ZTS_SHORT!-%PHP_BUILD_CRT%-%PHP_SDK_ARCH%\ /y /f
		7z a php_win32service-%PHP_REL%-!ZTS_SHORT!-%PHP_BUILD_CRT%-%PHP_SDK_ARCH%.zip %GITHUB_WORKSPACE%\php_win32service-%PHP_REL%-!ZTS_SHORT!-%PHP_BUILD_CRT%-%PHP_SDK_ARCH%\*
		@REM appveyor PushArtifact php_win32service-%PHP_REL%-!ZTS_SHORT!-%PHP_BUILD_CRT%-%PHP_SDK_ARCH%.zip -FileName php_win32service-%APPVEYOR_REPO_TAG_NAME%-%PHP_REL%-!ZTS_SHORT!-%PHP_BUILD_CRT%-%PHP_SDK_ARCH%.zip

		echo F|xcopy build\ext\php_win32service.dll artifacts\php_win32service-%PHP_REL%-!ZTS_SHORT!-%PHP_BUILD_CRT%-%PHP_SDK_ARCH%.dll /y /f
	)
endlocal
