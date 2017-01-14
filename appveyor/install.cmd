@echo off
setlocal enableextensions enabledelayedexpansion
	cinst wget
	mkdir C:\projects\win32service\build
	if not exist "%SDK_CACHE%" (
		echo Cloning remote SDK repository
		echo git clone -q --branch %SDK_BRANCH% %SDK_REMOTE% "%SDK_CACHE%"
		git clone -q --depth=1 --branch %SDK_BRANCH% %SDK_REMOTE% "%SDK_CACHE%"
	) else (
		echo Fetching remote SDK repository
		git --git-dir="%SDK_CACHE%\.git" --work-tree="%SDK_CACHE%" fetch --prune origin
		echo Checkout SDK repository branch
		git --git-dir="%SDK_CACHE%\.git" --work-tree="%SDK_CACHE%" checkout --force %SDK_BRANCH%
	)

	echo git clone -q --depth=1 --branch=PHP-%PHP_REL% https://github.com/php/php-src C:\projects\php-src
	git clone -q --depth=1 --branch=PHP-%PHP_REL% https://github.com/php/php-src C:\projects\php-src

	xcopy %APPVEYOR_BUILD_FOLDER% C:\projects\php-src\ext\win32service\ /s /e /y /f

	xcopy %APPVEYOR_BUILD_FOLDER%\LICENSE %APPVEYOR_BUILD_FOLDER%\artifacts\ /y /f
	xcopy %APPVEYOR_BUILD_FOLDER%\examples %APPVEYOR_BUILD_FOLDER%\artifacts\examples\ /y /f

	rem cd %APPVEYOR_BUILD_FOLDER%\appveyor
	rem wget -N --progress=bar:force:noscroll http://windows.php.net/downloads/php-sdk/php-sdk-binary-tools-20110915.zip
	rem 7z x -y php-sdk-binary-tools-20110915.zip -oC:\projects\php-sdk

endlocal