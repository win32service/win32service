@echo off

set NO_INTERACTION=1
set REPORT_EXIT_STATUS=1
set SKIP_IO_CAPTURE_TESTS=1

if /i "%PHP_REL%" equ "master" (
	set BRANCH=master
	set STABILITY=staging
) else (
	set BRANCH=%PHP_REL%
	set STABILITY=stable
)
set DEPS_DIR=%PHP_BUILD_CACHE_BASE_DIR%\deps-%BRANCH%-%PHP_SDK_VC%-%PHP_SDK_ARCH%
if not exist "%DEPS_DIR%" (
	echo "%DEPS_DIR%" doesn't exist
	exit /b 3
)

rem prepare for ext/openssl
@REM if "%APPVEYOR%" equ "True" rmdir /s /q C:\OpenSSL-Win32 >NUL 2>NUL
@REM if "%APPVEYOR%" equ "True" rmdir /s /q C:\OpenSSL-Win64 >NUL 2>NUL
mkdir c:\usr\local\ssl
if %errorlevel% neq 0 exit /b 3
copy %DEPS_DIR%\template\ssl\openssl.cnf c:\usr\local\ssl
if %errorlevel% neq 0 exit /b 3
set OPENSSL_CONF=c:\usr\local\ssl\openssl.cnf
rem set OPENSSL_CONF=
rem set SSLEAY_CONF=

mkdir c:\tests_tmp

set TEST_PHP_JUNIT=c:\junit.out.xml

if /i "%PHP_REL%" equ "7.2" (
	set ADDOPT=--show-slow 1000
)
if /i "%PHP_REL%" equ "7.3" (
	set ADDOPT=--show-slow 1000
)
if /i "%PHP_REL%" equ "7.4" (
	set ADDOPT=--show-slow 1000
)
if /i "%PHP_REL%" equ "8.0" (
	set ADDOPT=--show-slow 1000
)

rem remove old service
sc delete WindowsServicePhpTest
sc delete WindowsServicePhpTestRun

rem cd "%APPVEYOR_BUILD_FOLDER%"
nmake test TESTS="-q --offline --show-diff %ADDOPT% --set-timeout 120 -g FAIL,XFAIL,BORK,WARN,LEAK,SKIP --temp-source c:\tests_tmp --temp-target c:\tests_tmp"

set EXIT_CODE=%errorlevel%

@REM powershell -Command "$wc = New-Object 'System.Net.WebClient'; $wc.UploadFile('https://ci.appveyor.com/api/testresults/junit/%APPVEYOR_JOB_ID%', 'c:\junit.out.xml')"

exit /b %EXIT_CODE%
