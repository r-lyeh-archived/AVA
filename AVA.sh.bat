#/bin/bash 2>nul || goto :windows



# bash
echo [bash env]

tools/builder/premake5.linux codelite
tools/builder/premake5.linux gmake
tools/builder/premake5.linux vs2013
tools/builder/premake5.linux xcode4
tools/builder/premake5.linux ninja
tools/builder/ninja.linux -C .project

tools/builder/premake5.osx codelite
tools/builder/premake5.osx gmake
tools/builder/premake5.osx vs2013
tools/builder/premake5.osx xcode4
tools/builder/premake5.osx ninja
tools/builder/ninja.osx   -C .project

exit





:windows
@echo off

    REM cleanup

        if "%1"=="wipe" (
            if exist .build rd /q /s .build && if exist .build echo "error cannot clean up .build" && exit /b
            if exist .project rd /q /s .project && if exist .project echo "error cannot clean up .project" && exit /b
            echo Clean up && exit /b
        )

    REM setup

        REM MSVC
        if not "%Platform%"=="x64" (
            echo [win][msc]
                   if exist "%VS140COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat" (
                      @call "%VS140COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
            ) else if exist "%VS120COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat" (
                      @call "%VS120COMNTOOLS%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
            ) else (
                echo Warning: Could not find x64 environment variables for Visual Studio 2013/2015
                exit /b
            )
        )

        REM Luajit, %AVAROOT%
        if ""=="" (
            echo [win][set]
            set AVAROOT="%~dp0%\"
        )

    REM build

        pushd "%AVAROOT%\tools\builder"

            REM project generation
            REM premake5.exe codelite
            REM premake5.exe gmake
            REM premake5.exe vs2015
            REM premake5.exe vs2013
            REM premake5.exe xcode4
            premake5.exe ninja

            REM actual build
            set NINJA_STATUS="[%%e] [%%r/%%f]"
            ninja.exe -v -C ..\..\.project
            set OK=%ERRORLEVEL%

        popd

    REM launch

    pushd "%AVAROOT%"

        if "0"=="%OK%" (
            echo ^>^> launcher
            .build\debug\launcher.exe %*
            echo ^<^< launcher
        ) else (
            echo  && rem beep
        )

    popd

    REM exit

        echo Press any key to continue... && pause > nul
        exit /b
