@echo off
REM Build and run a C++ file

REM Set the C++ file name
set CPP_FILE=main.cpp

REM Set the output directory
set OUT_DIR=bin

REM Ensure the output directory exists
if not exist %OUT_DIR% mkdir %OUT_DIR%

REM Set the output executable name within the bin directory
set EXE_FILE=%OUT_DIR%\main.exe

REM Compile the C++ file
g++ %CPP_FILE% -o %EXE_FILE%

REM Check if compilation was successful
if %ERRORLEVEL% neq 0 (
    echo Compilation failed.
    exit /b %ERRORLEVEL%
)

REM Run the executable
.\%EXE_FILE%