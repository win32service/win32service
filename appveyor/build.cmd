@echo off
setlocal enableextensions enabledelayedexpansion
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

		call !SDK_RUNNER! -t %APPVEYOR_BUILD_FOLDER%\appveyor\build_task.cmd

		if %errorlevel% neq 0 exit /b 3

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