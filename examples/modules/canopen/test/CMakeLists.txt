cmake_minimum_required(VERSION 3.13.4)

project(example_canopen_test)

set(test_files  src/main.c
                src/ut_canopen_test.c
                ../src/canopen.c
                ../src/canopen_functions.c
                ../src/sdo.c
                ../src/sdo_functions.c
                ../../interfaces/src/sys_if.c
                ../../../../../crf/src/chsm.c
                ../../../../../crf/src/cvalue.c
                ../../../../../crf/src/cpool.c
                ../../../../../crf/src/cqueue.c
                ../../../../../crf/src/crf.c
                ../../../../../crf/src/cbits.c
                ../../../../../unity/extras/fixture/src/unity_fixture.c
                ../../../../../unity/extras/memory/src/unity_memory.c
                ../../../../../unity/src/unity.c
                )

include_directories(tinc
                inc
                ../inc
                ../../interfaces/inc
                ../../../../crf/inc
                ../../../../../unity/extras/fixture/src
                ../../../../../unity/extras/memory/src
                ../../../../../unity/src
                )


add_executable(canopen_test ${test_files})

add_compile_options(-Wall -Wextra -pedantic -Werror)

add_compile_definitions(NDEBUG)
