@echo off
setlocal enableextensions enabledelayedexpansion
	cinst wget

	if not exist "%PHP_BUILD_CACHE_BASE_DIR%" (
		echo Creating %PHP_BUILD_CACHE_BASE_DIR%
		mkdir "%PHP_BUILD_CACHE_BASE_DIR%"
	)

	if not exist "%PHP_BUILD_OBJ_DIR%" (
		echo Creating %PHP_BUILD_OBJ_DIR%
		mkdir "%PHP_BUILD_OBJ_DIR%"
	)

	if not exist "%PHP_BUILD_CACHE_SDK_DIR%" (
		echo Cloning remote SDK repository
		rem git clone -q --depth=1 --branch %SDK_BRANCH% %SDK_REMOTE% "%PHP_BUILD_CACHE_SDK_DIR%" 2>&1
		git clone --branch %SDK_BRANCH% %SDK_REMOTE% "%PHP_BUILD_CACHE_SDK_DIR%" 2>&1
	) else (
		echo Fetching remote SDK repository
		git --git-dir="%PHP_BUILD_CACHE_SDK_DIR%\.git" --work-tree="%PHP_BUILD_CACHE_SDK_DIR%" fetch --prune origin 2>&1
		echo Checkout SDK repository branch
		git --git-dir="%PHP_BUILD_CACHE_SDK_DIR%\.git" --work-tree="%PHP_BUILD_CACHE_SDK_DIR%" checkout --force %SDK_BRANCH%
	)


	if "%PHP_REL%"=="master" (
		echo git clone -q --depth=1 --branch=%PHP_REL% https://github.com/php/php-src C:\projects\php-src
		git clone --depth=1 --branch=%PHP_REL% https://github.com/php/php-src C:\projects\php-src
	) else (
		echo git clone -q --depth=1 --branch=PHP-%PHP_REL% https://github.com/php/php-src C:\projects\php-src
		git clone --depth=1 --branch=PHP-%PHP_REL% https://github.com/php/php-src C:\projects\php-src
	)

	xcopy %GITHUB_WORKSPACE% C:\projects\php-src\ext\win32service\ /s /e /y /f

	xcopy %GITHUB_WORKSPACE%\LICENSE %GITHUB_WORKSPACE%\artifacts\ /y /f
	xcopy %GITHUB_WORKSPACE%\examples %GITHUB_WORKSPACE%\artifacts\examples\ /y /f

	@REM if "%APPVEYOR%" equ "True" rmdir /s /q C:\cygwin >NUL 2>NUL
	@REM if %errorlevel% neq 0 exit /b 3
	@REM if "%APPVEYOR%" equ "True" rmdir /s /q C:\cygwin64 >NUL 2>NUL
	@REM if %errorlevel% neq 0 exit /b 3
	@REM if "%APPVEYOR%" equ "True" rmdir /s /q C:\mingw >NUL 2>NUL
	@REM if %errorlevel% neq 0 exit /b 3
	@REM if "%APPVEYOR%" equ "True" rmdir /s /q C:\mingw-w64 >NUL 2>NUL
	@REM if %errorlevel% neq 0 exit /b 3

	if "%GITHUB_REF_TYPE%"=="branch" if NOT "%GITHUB_HEAD_REF%"=="" (
	    echo use head ref %GITHUB_HEAD_REF%
		set REF_NAME=%GITHUB_HEAD_REF:/=-%
	) else (
	    echo use ref name %GITHUB_REF_NAME%
		set REF_NAME=%GITHUB_REF_NAME:/=-%
	)

	if "%APPVEYOR_REPO_TAG_NAME%"=="" (
		set APPVEYOR_REPO_TAG_NAME=!REF_NAME!-%GITHUB_SHA:~0,8%
		for /f "delims=" %%l in (php_win32service.h) do (
			if not "%%l"=="" (
				set line=%%l
				if "!line:~8,24!"=="PHP_WIN32SERVICE_VERSION" (
					set APPVEYOR_REPO_TAG_NAME=!line:~34,-1!-!REF_NAME!-%GITHUB_SHA:~0,8%
				)
			)
		)
		echo "::set-output name=repotagname::!APPVEYOR_REPO_TAG_NAME!"
		echo repotagname=!APPVEYOR_REPO_TAG_NAME!> %GITHUB_ENV%
	)
    if "%Platform%"=="x86" (
    echo vcvars32.bat
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"
    ) else (
    echo vcvars64.bat
	call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
    )
    set
endlocal
