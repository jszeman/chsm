cmake_minimum_required(VERSION 3.13.4)

project(CrfTest)

set(test_files  tsrc/main.c
                tsrc/cpool_test.c
                tsrc/cqueue_test.c
                tsrc/chsm_test.c
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
