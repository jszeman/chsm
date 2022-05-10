@echo off

cd build

cmake --build . -t crf_test 
ctest -VV -R crf_test