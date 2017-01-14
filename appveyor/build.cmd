@echo off
setlocal enableextensions enabledelayedexpansion
	for %%a in (%ARCHITECTURES%) do (
		set ARCH=%%a

		set SDK_RUNNER=%SDK_CACHE%\phpsdk-vc14-!ARCH!.bat
		if not exist "!SDK_RUNNER!" (
			echo "!SDK_RUNNER!" doesn't exist
			exit /b 3
		)

		call !SDK_RUNNER! -t %APPVEYOR_BUILD_FOLDER%\appveyor\build_task.cmd

		if %errorlevel% neq 0 exit /b 3

	)
endlocal