@echo off
setlocal enableextensions enabledelayedexpansion
	if not exist "%SDK_CACHE%" (
		echo Cloning remote SDK repository
		git clone --branch %SDK_BRANCH% %SDK_REMOTE% "%SDK_CACHE%" 2>&1
	) else (
		echo Fetching remote SDK repository
		git --git-dir="%SDK_CACHE%\.git" --work-tree="%SDK_CACHE%" fetch --prune origin 2>&1
		echo Checkout SDK repository branch
		git --git-dir="%SDK_CACHE%\.git" --work-tree="%SDK_CACHE%" checkout --force %SDK_BRANCH%
	)

	echo git clone -q --branch=PHP-%PHP_REL% https://github.com/php/php-src C:\projects\php-src
	git clone --branch=PHP-%PHP_REL% https://github.com/php/php-src C:\projects\php-src

	xcopy %APPVEYOR_BUILD_FOLDER% C:\projects\php-src\ext\win32service\ /s /e /y /f

	xcopy %APPVEYOR_BUILD_FOLDER%\LICENSE %APPVEYOR_BUILD_FOLDER%\artifacts\ /y /f
	xcopy %APPVEYOR_BUILD_FOLDER%\examples %APPVEYOR_BUILD_FOLDER%\artifacts\examples\ /y /f

	rem cd %APPVEYOR_BUILD_FOLDER%\appveyor
	rem wget -N --progress=bar:force:noscroll http://windows.php.net/downloads/php-sdk/php-sdk-binary-tools-20110915.zip
	rem 7z x -y php-sdk-binary-tools-20110915.zip -oC:\projects\php-sdk

	for %%a in (%ARCHITECTURES%) do (
		set ARCH=%%a
		rem if "!ARCH!"=="amd64" set DEPTS_ARCH=x64
		rem if "!ARCH!"=="x86" set DEPTS_ARCH=x86

		rem call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall" !ARCH!
		rem call C:\projects\php-sdk\bin\phpsdk_setvars.bat
		set SDK_RUNNER=%SDK_CACHE%\phpsdk-vc14-!ARCH!.bat
		if not exist "!SDK_RUNNER!" (
			echo "!SDK_RUNNER!" doesn't exist
			exit /b 3
		)

		call !SDK_RUNNER!

		cd %APPVEYOR_BUILD_FOLDER%\appveyor
		wget -N --progress=bar:force:noscroll http://windows.php.net/downloads/php-sdk/deps-%PHP_REL%-vc14-!ARCH!.7z -P %CACHE_ROOT%
		7z x -y deps-%PHP_REL%-vc14-!DEPTS_ARCH!.7z -o%CACHE_ROOT%

		for %%z in (%ZTS_STATES%) do (
			set ZTS_STATE=%%z
			if "!ZTS_STATE!"=="enable" set ZTS_SHORT=ts
			if "!ZTS_STATE!"=="disable" set ZTS_SHORT=nts

			cd C:\projects\php-src

			call buildconf.bat

			if %errorlevel% neq 0 exit /b 3

			call configure.bat --disable-all --with-mp=auto --enable-cli --!ZTS_STATE!-zts --enable-win32service=shared --with-config-file-scan-dir=%APPVEYOR_BUILD_FOLDER%\build\modules.d --with-prefix=%APPVEYOR_BUILD_FOLDER%\build --with-php-build=%CACHE_ROOT%\deps

			if %errorlevel% neq 0 exit /b 3

			nmake

			if %errorlevel% neq 0 exit /b 3

			nmake install

			if %errorlevel% neq 0 exit /b 3

			cd %APPVEYOR_BUILD_FOLDER%

			if not exist build\ext\php_win32service.dll exit /b 3

			move build\ext\php_win32service.dll artifacts\php_win32service-%PHP_REL%-vc14-!ZTS_SHORT!-!DEPTS_ARCH!.dll
		)
	)

	if "%APPVEYOR_REPO_TAG_NAME%"=="" (
		set APPVEYOR_REPO_TAG_NAME=%APPVEYOR_REPO_BRANCH%-%APPVEYOR_REPO_COMMIT:~0,8%
		for /f "delims=" %%l in (php_win32service.h) do (
			if not "%%l"=="" (
				set line=%%l
				if "!line:~8,24!"=="PHP_WIN32SERVICE_VERSION" (
					set APPVEYOR_REPO_TAG_NAME=!line:~34,-1!-%APPVEYOR_REPO_BRANCH%-%APPVEYOR_REPO_COMMIT:~0,8%
				)
			)
		)

		appveyor SetVariable -Name APPVEYOR_REPO_TAG_NAME -Value !APPVEYOR_REPO_TAG_NAME!
	)
endlocal