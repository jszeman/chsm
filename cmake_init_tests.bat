@echo off

set current_path=%cd%
set cpu_architecture=mingw

if exist "%~dp0/build-%cpu_architecture%-test" (
    echo directory exist
    cd build-%cpu_architecture%-test
) else (
    echo directory doesen't exist
    mkdir build-%cpu_architecture%-test
    cd build-%cpu_architecture%-test
)

cmake -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=%current_path%\cmake\compilers\toolchain-mingw.cmake -DCHSM_BUILD_TESTS=ON ../

