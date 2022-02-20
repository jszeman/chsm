cmake_minimum_required(VERSION 3.19)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}../../../../cmake/utils")

include(Fetch_unity)

add_executable(intf_test 
                src/main.c
                src/ut_i2c_driver_mock.c
                src/ut_i2c_driver_mock_test.c
                src/ut_spi_driver_mock.c
                src/ut_spi_driver_mock_test.c
                ../../../crf/src/chsm.c
                ../../../crf/src/cvalue.c
                ../../../crf/src/cpool.c
                ../../../crf/src/cqueue.c
                ../../../crf/src/crf.c
                ../../../crf/src/cbits.c
                )

target_include_directories(intf_test PRIVATE
                tinc
                inc
                ../inc
                ../../../crf/inc
                )

target_compile_definitions(intf_test PRIVATE 
                NDEBUG
                SIGNAL_CLASSES_H=\"${CMAKE_CURRENT_SOURCE_DIR}/inc/signal_classes.h\")

target_compile_options(intf_test PRIVATE 
    -Wall 
    -Wextra 
    -pedantic 
    # -Werror
    )
target_link_options(intf_test PRIVATE
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map   
)

target_link_libraries(intf_test PRIVATE unity)

set(debug ON)

include(diagnostic)
diagnostic(intf_test)

enable_testing()

add_test(NAME intf_test
        COMMAND intf_test
       )