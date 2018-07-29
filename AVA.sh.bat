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

    REM rebuild

        if "%1"=="rebuild" (
            call "%0" clean
            call "%0" 
            exit /b
        )


    REM cleanup

        if "%1"=="clean" (
            if exist _build   rd /q /s _build   && if exist _build   echo "error cannot clean up _build" && exit /b
            if exist _project rd /q /s _project && if exist _project echo "error cannot clean up _project" && exit /b
            echo Clean up && exit /b
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
                   if exist "%VS150COMNTOOLS%\..\..\VC\Auxiliary\Build\vcvarsx86_amd64.bat" (
                      @call "%VS150COMNTOOLS%\..\..\VC\Auxiliary\Build\vcvarsx86_amd64.bat" 
            ) else if exist "%VS140COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat" (
                      @call "%VS140COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
            ) else if exist "%VS120COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat" (
                      @call "%VS120COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
            ) else (
                echo Warning: Could not find x64 environment variables for Visual Studio 2017/2015/2013
                exit /b
            )
            set Platform=x64
        )

        REM Luajit, %AVAROOT%
        if ""=="" (
            echo [win][set]
            set AVAROOT="%~dp0%\"
            REM set      path="%path%;%~dp0%\bin\;"
            REM endlocal &&  && set AVAROOT=%AVAROOT%
        )

    REM build

        pushd "%AVAROOT%\tools\builder"

            REM project generation
            REM premake5.exe codelite
            REM premake5.exe gmake
            premake5.exe vs2015
            REM premake5.exe vs2013
            REM premake5.exe xcode4
            premake5.exe ninja

            REM actual build
            set NINJA_STATUS="[%%e] [%%r/%%f]"
            ninja.exe -v -C ..\..\_project
            set OK=%ERRORLEVEL%

        popd

    REM launch

        pushd "%AVAROOT%"

            if "0"=="%OK%" (
                color
                echo ^>^> launch
                _build\debug\launch.exe %*
                echo ^<^< launch
            ) else (
                color 4f
                echo  && rem beep
            )

        popd

    REM exit

        echo Press any key to continue... && pause > nul
        color
        exit /b
