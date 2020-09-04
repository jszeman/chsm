cmake_minimum_required(VERSION 3.13.4)

project(crf_lib)

set(lib_files   src/chsm.c
                src/cpool.c
                src/cqueue.c
                src/crf.c
                )

include_directories(inc)

add_library(crf ${lib_files})

add_compile_definitions(NDEBUG)
