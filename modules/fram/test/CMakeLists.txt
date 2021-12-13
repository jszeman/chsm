cmake_minimum_required(VERSION 3.19)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}../../../../cmake/utils")

include(Fetch_unity)

add_executable(fram_test 
                src/main.c
                src/ut_fram_test.c
                ../src/fram.c
                ../src/fram_functions.c
                ../../i2c_master/src/i2c_master.c
                ../../i2c_master/src/i2c_master_functions.c
                ../../interfaces/test/src/ut_i2c_driver_mock.c
                ../../../crf/src/chsm.c
                ../../../crf/src/cvalue.c
                ../../../crf/src/cpool.c
                ../../../crf/src/cqueue.c
                ../../../crf/src/crf.c
                ../../../crf/src/cbits.c
                )

target_include_directories(fram_test PRIVATE
                tinc
                inc
                ../inc
                ../../i2c_master/inc
                ../../interfaces/test/inc
                ../../interfaces/inc
                ../../../crf/inc
                )

target_compile_definitions(fram_test PRIVATE 
                NDEBUG
                SIGNAL_CLASSES_H="${CMAKE_CURRENT_SOURCE_DIR}/inc/signal_classes.h")

target_compile_options(fram_test PRIVATE 
    -Wall 
    -Wextra 
    -pedantic 
    # -Werror
    )
target_link_options(fram_test PRIVATE
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map   
)

target_link_libraries(fram_test PRIVATE unity)

set(debug ON)

include(diagnostic)
diagnostic(fram_test)

enable_testing()

add_test(NAME fram_test
        COMMAND fram_test
       )