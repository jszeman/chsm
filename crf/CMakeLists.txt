cmake_minimum_required(VERSION 3.19)

project(crf_lib 
        LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../../cmake/utils")

add_library(crf   
                src/cbits.c
                src/chsm.c
                src/cpool.c
                src/cqueue.c
                src/crf.c
                src/cvalue.c
                )
                
target_include_directories(crf PUBLIC
                inc
                ../modules/interfaces/inc)

target_compile_definitions(crf PUBLIC 
                NDEBUG
)

include(diagnostic)
set(debug on)
set(autodetect_toolchain on)
diagnostic(crf)