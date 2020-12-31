cmake_minimum_required(VERSION 3.13.4)

project(example_lm73_test)

set(test_files  src/main.c
                src/ut_lm73_test.c
                ../src/lm73.c
                ../src/lm73_functions.c
                ../../i2c_master/src/i2c_master.c
                ../../i2c_master/src/i2c_master_functions.c
                ../../interfaces/test/src/ut_i2c_driver_mock.c
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
                ../../i2c_master/inc
                ../../interfaces/test/inc
                ../../interfaces/inc
                ../../../../crf/inc
                ../../../../../unity/extras/fixture/src
                ../../../../../unity/extras/memory/src
                ../../../../../unity/src
                )


add_executable(lm73_test ${test_files})

add_compile_options(-Wall -Wextra -pedantic -Werror)

add_compile_definitions(NDEBUG)
