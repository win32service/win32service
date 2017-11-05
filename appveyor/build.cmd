@echo off
setlocal enableextensions enabledelayedexpansion
	for %%a in (%ARCHITECTURES%) do (
		set ARCH=%%a

		set SDK_RUNNER=%PHP_BUILD_CACHE_SDK_DIR%\phpsdk-%PHP_BUILD_CRT%-!ARCH!.bat
		if not exist "!SDK_RUNNER!" (
			echo "!SDK_RUNNER!" doesn't exist
			exit /b 3
		)

		cmd /c !SDK_RUNNER! -t %APPVEYOR_BUILD_FOLDER%\appveyor\build_task.cmd

		if %errorlevel% neq 0 exit /b 3

	)
endlocal