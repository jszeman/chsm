cmake_minimum_required(VERSION 3.13.4)

project(CrfTest)

set(test_files  tsrc/main.c
                tsrc/cpool_test.c
                tsrc/cqueue_test.c
                tsrc/chsm_test.c
                tsrc/crf_test.c
                tsrc/chsm_test_functions.c
                tsrc/chsm_test_machine.c
                tsrc/chsm_test_machine2.c
                tsrc/chsm_test_machine3.c
                tsrc/chsm_test_machine4.c
                tsrc/bus_driver.c
                tsrc/bus_driver_functions.c
                tsrc/dev_driver.c
                tsrc/dev_driver_functions.c
                ../src/chsm.c
                ../src/cpool.c
                ../src/cqueue.c
                ../src/crf.c
                ../../../unity/extras/fixture/src/unity_fixture.c
                ../../../unity/extras/memory/src/unity_memory.c
                ../../../unity/src/unity.c
                )

include_directories(tinc
                    ../inc
                    ../../../unity/extras/fixture/src
                    ../../../unity/extras/memory/src
                    ../../../unity/src)


add_executable(crf_test ${test_files})

add_compile_options(-Wall -Wextra -pedantic -Werror)

add_compile_definitions(NDEBUG)
