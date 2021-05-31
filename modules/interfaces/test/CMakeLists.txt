cmake_minimum_required(VERSION 3.13.4)

project(example_intf_test)

set(test_files  src/main.c
                src/ut_i2c_driver_mock.c
                src/ut_i2c_driver_mock_test.c
                src/ut_spi_driver_mock.c
                src/ut_spi_driver_mock_test.c
                ../../../../crf/src/chsm.c
                ../../../../crf/src/cvalue.c
                ../../../../crf/src/cpool.c
                ../../../../crf/src/cqueue.c
                ../../../../crf/src/crf.c
                ../../../../crf/src/cbits.c
                ../../../../unity/extras/fixture/src/unity_fixture.c
                ../../../../unity/extras/memory/src/unity_memory.c
                ../../../../unity/src/unity.c
                )

include_directories(tinc
                inc
                ../inc
                ../../../crf/inc
                ../../../../unity/extras/fixture/src
                ../../../../unity/extras/memory/src
                ../../../../unity/src
                )


add_executable(intf_test ${test_files})

add_compile_options(-Wall -Wextra -pedantic -Werror)

add_compile_definitions(NDEBUG)
add_compile_definitions(SIGNAL_CLASSES_H="signal_classes.h")