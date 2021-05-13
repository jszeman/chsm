cmake_minimum_required(VERSION 3.13.4)

project(example_spi_master_test)

set(test_files  src/main.c
                src/ut_spi_master_test.c
                ../src/spi_master.c
                ../src/spi_master_functions.c
                ../../interfaces/test/src/ut_spi_driver_mock.c
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
                ../../interfaces/test/inc
                ../../interfaces/inc
                ../../../../crf/inc
                ../../../../../unity/extras/fixture/src
                ../../../../../unity/extras/memory/src
                ../../../../../unity/src
                )


add_executable(spi_master_test ${test_files})

add_compile_options(-Wall -Wextra -pedantic -Werror)

add_compile_definitions(NDEBUG)
add_compile_definitions(SIGNAL_CLASSES_H="signal_classes.h")
