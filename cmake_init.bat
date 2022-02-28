@echo off

if exist "%~dp0/build" (
    echo directory exist
    cd build
) else (
    echo directory doesen't exist
    mkdir build
    cd build
)

cmake -G "MinGW Makefiles" ../

