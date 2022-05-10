@echo off

set current_path=%cd%
set cpu_architecture=mingw

if exist "%~dp0/build-%cpu_architecture%" (
    echo directory exist
    cd build-%cpu_architecture%
) else (
    echo directory doesen't exist
    mkdir build-%cpu_architecture%
    cd build-%cpu_architecture%
)

cmake -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=%current_path%\cmake\compilers\toolchain-mingw.cmake ../

