#/bin/bash 2>nul || goto :windows

# bash
echo [bash env]

# cd "$(dirname -- "$(readlink -fn -- "$0")")"
cd `dirname $0`

bin/builder/premake5.linux codelite
bin/builder/premake5.linux gmake
bin/builder/premake5.linux vs2013
bin/builder/premake5.linux xcode4
bin/builder/premake5.linux ninja
bin/builder/ninja.linux -C _project

bin/builder/premake5.osx codelite
bin/builder/premake5.osx gmake
bin/builder/premake5.osx vs2013
bin/builder/premake5.osx xcode4
bin/builder/premake5.osx ninja
bin/builder/ninja.osx   -C _project

exit





:windows
@echo off

    attrib +s editor 1> nul 2> nul

    REM rebuild

        if "%1"=="rebuild" (
            call "%0" clean
            call "%0" 
            exit /b
        )

    REM cleanup

        if "%1"=="clean" (
            if exist _debug    rd /q /s _debug    && if exist _debug    echo "error cannot clean up _debug"    && goto error
            if exist _debugopt rd /q /s _debugopt && if exist _debugopt echo "error cannot clean up _debugopt" && goto error
            if exist _release  rd /q /s _release  && if exist _release  echo "error cannot clean up _release"  && goto error
            if exist _project  rd /q /s _project  && if exist _project  echo "error cannot clean up _project"  && goto error
            echo Cleaning up. && exit /b
        )

        if "%1"=="fuse" (
            REM bundle a game-redist.zip #framework\*.dll
            REM bundle a output.zip * -x#* -x.*
            REM copy /b ava.exe+output.zip game.exe
        )

    REM setup

        REM MSVC
        if not "%Platform%"=="x64" (
            echo [win][msc]
                   if exist "%VS160COMNTOOLS%\..\..\VC\Auxiliary\Build\vcvarsx86_amd64.bat" (
                      @call "%VS160COMNTOOLS%\..\..\VC\Auxiliary\Build\vcvarsx86_amd64.bat" 
            ) else if exist "%VS150COMNTOOLS%\..\..\VC\Auxiliary\Build\vcvarsx86_amd64.bat" (
                      @call "%VS150COMNTOOLS%\..\..\VC\Auxiliary\Build\vcvarsx86_amd64.bat" 
            ) else if exist "%VS140COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat" (
                      @call "%VS140COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
            ) else if exist "%VS120COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat" (
                      @call "%VS120COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
            ) else if exist "%ProgramFiles(x86)%\microsoft visual studio\2017\community\VC\Auxiliary\Build\vcvarsx86_amd64.bat" (
                      @call "%ProgramFiles(x86)%\microsoft visual studio\2017\community\VC\Auxiliary\Build\vcvarsx86_amd64.bat"
            ) else (
                echo Error: Could not find x64 environment variables for Visual Studio 2017/2015/2013
                goto error
            )
            set Platform=x64
        )

        REM Luajit, %AVAROOT%
        if ""=="" (
            echo [win][set]
            REM set AVAROOT="%~dp0%\"
            REM set      path="%path%;%~dp0%\bin\;"
            REM endlocal &&  && set AVAROOT=%AVAROOT%
        )

    REM build

        call "%0" clean

        pushd "%~dp0%\tools\builder"

            REM project generation
            REM premake5.exe codelite
            REM premake5.exe gmake
            ..\colorizer\colorizer premake5.exe vs2013
            REM premake5.exe vs2013
            REM premake5.exe xcode4
            ..\colorizer\colorizer premake5.exe ninja

            REM actual build
            set NINJA_STATUS="[%%r/%%f] [%%e] "
            if "%1"=="debugopt" ( ..\colorizer\colorizer ninja.exe -v -C ..\..\_project debugopt ) else ( ^
            if "%1"=="release"  ( ..\colorizer\colorizer ninja.exe -v -C ..\..\_project release  ) else ( ^
                                  ..\colorizer\colorizer ninja.exe -v -C ..\..\_project                   ^
            ) )
            set OK=%ERRORLEVEL%

        popd

    REM editor

        pushd "%~dp0%"

            if exist _debug\editor.exe    copy /y engine\3rd\SDL2.dll _debug 1>2> nul
            if exist _release\editor.exe  copy /y engine\3rd\SDL2.dll _release 1>2> nul
            if exist _debugopt\editor.exe copy /y engine\3rd\SDL2.dll _debugopt 1>2> nul

            if "0"=="%OK%" (
                rem color
                echo ^>^> editor

                if "%1"=="release" shift
                if "%1"=="debugopt" shift

                if "%1"=="debug" (
                    if exist _debug\editor.exe    start "" devenv /Run "_debug\editor.exe" %*
                    if exist _release\editor.exe  start "" devenv /Run "_release\editor.exe" %*
                    if exist _debugopt\editor.exe start "" devenv /Run "_debugopt\editor.exe" %*
                ) else (
                    if exist _debug\editor.exe    pushd _debug && editor.exe %* && popd
                    if exist _release\editor.exe  pushd _release && editor.exe %* && popd
                    if exist _debugopt\editor.exe pushd _debugopt && editor.exe %* && popd
                )

                echo ^<^< editor
            ) else (
                :error
                rem color 4f
                rem echo  && rem beep
            )

        popd

    REM exit
        echo Press any key to continue... && pause > nul
        rem color
        exit /b
