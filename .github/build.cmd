@echo off
echo APPVEYOR_REPO_TAG_NAME="%APPVEYOR_REPO_TAG_NAME%"
setlocal enableextensions enabledelayedexpansion
rem	for %%a in (%ARCHITECTURES%) do (
		set ARCH=%PLATFORM%
		set SDK_RUNNER=%PHP_BUILD_CACHE_SDK_DIR%\phpsdk-%PHP_BUILD_CRT%-!ARCH!.bat
		if not exist "!SDK_RUNNER!" (
			echo "!SDK_RUNNER!" doesn't exist
			exit /b 3
		)

		cmd /c !SDK_RUNNER! -t %GITHUB_WORKSPACE%\.github\build_task.cmd

		if %errorlevel% neq 0 exit /b 3

rem	)
endlocal
