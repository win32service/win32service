@echo off
setlocal enableextensions enabledelayedexpansion
	wget -N --progress=bar:force:noscroll http://windows.php.net/downloads/php-sdk/deps-%PHP_REL%-vc14-!ARCH!.7z -P %CACHE_ROOT%
	7z x -y %CACHE_ROOT%\deps-%PHP_REL%-vc14-%ARCH%.7z -oC:\projects\php-src

	for %%z in (%ZTS_STATES%) do (
		set ZTS_STATE=%%z
		if "!ZTS_STATE!"=="enable" set ZTS_SHORT=ts
		if "!ZTS_STATE!"=="disable" set ZTS_SHORT=nts

		cd /d C:\projects\php-src

		call buildconf.bat

		if %errorlevel% neq 0 exit /b 3

		call configure.bat --disable-all --with-mp=auto --enable-cli --!ZTS_STATE!-zts --enable-win32service=shared --with-config-file-scan-dir=%APPVEYOR_BUILD_FOLDER%\build\modules.d --with-prefix=%APPVEYOR_BUILD_FOLDER%\build --with-php-build=deps

		if %errorlevel% neq 0 exit /b 3

		nmake

		if %errorlevel% neq 0 exit /b 3

		nmake install

		if %errorlevel% neq 0 exit /b 3

		cd /d %APPVEYOR_BUILD_FOLDER%

		if not exist "%APPVEYOR_BUILD_FOLDER%\build\ext\php_win32service.dll" exit /b 3

		xcopy %APPVEYOR_BUILD_FOLDER%\LICENSE %APPVEYOR_BUILD_FOLDER%\php_win32service-%PHP_REL%-vc14-!ZTS_SHORT!-%ARCH%\ /y /f
		xcopy %APPVEYOR_BUILD_FOLDER%\examples %APPVEYOR_BUILD_FOLDER%\php_win32service-%PHP_REL%-vc14-!ZTS_SHORT!-%ARCH%\examples\ /y /f
		xcopy %APPVEYOR_BUILD_FOLDER%\build\ext\php_win32service.dll %APPVEYOR_BUILD_FOLDER%\php_win32service-%PHP_REL%-vc14-!ZTS_SHORT!-%ARCH%\ /y /f
		7z a php_win32service-%PHP_REL%-vc14-!ZTS_SHORT!-%ARCH%.zip %APPVEYOR_BUILD_FOLDER%\php_win32service-%PHP_REL%-vc14-!ZTS_SHORT!-%ARCH%\*
		appveyor PushArtifact appveyor php_win32service-%PHP_REL%-vc14-!ZTS_SHORT!-%ARCH%.zip

		move build\ext\php_win32service.dll artifacts\php_win32service-%PHP_REL%-vc14-!ZTS_SHORT!-%ARCH%.dll
	)
endlocal