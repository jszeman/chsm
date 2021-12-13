cmake_minimum_required(VERSION 3.19)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}../../../../cmake/utils")

include(Fetch_unity)

add_executable(dac8565_test 
                src/main.c
                src/dac8565_test.c
                ../../spi_master/src/spi_master.c
                ../../spi_master/src/spi_master_functions.c
                ../../interfaces/test/src/ut_spi_driver_mock.c
                ../../../crf/src/chsm.c
                ../../../crf/src/cvalue.c
                ../../../crf/src/cpool.c
                ../../../crf/src/cqueue.c
                ../../../crf/src/crf.c
                ../../../crf/src/cbits.c
                )

target_include_directories(dac8565_test PRIVATE
                tinc
                inc
                ../inc
                ../../spi_master/inc
                ../../interfaces/test/inc
                ../../interfaces/inc
                ../../../crf/inc
                )

target_compile_definitions(dac8565_test PRIVATE 
                NDEBUG
                SIGNAL_CLASSES_H="${CMAKE_CURRENT_SOURCE_DIR}/inc/signal_classes.h")

target_compile_options(dac8565_test PRIVATE 
    -Wall 
    -Wextra 
    -pedantic 
    # -Werror
    )
target_link_options(dac8565_test PRIVATE
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map   
)

target_link_libraries(dac8565_test PRIVATE unity)

diagnostic(dac8565_test)

enable_testing()

add_test(NAME dac8565_test
        COMMAND dac8565_test
       )