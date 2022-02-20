cmake_minimum_required(VERSION 3.19)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}../../../../cmake/utils")

include(Fetch_unity)

add_executable(spi_master_test 
                src/main.c
                src/ut_spi_master_test.c
                ../src/spi_master.c
                ../src/spi_master_functions.c
                ../../interfaces/test/src/ut_spi_driver_mock.c
                ../../../crf/src/chsm.c
                ../../../crf/src/cvalue.c
                ../../../crf/src/cpool.c
                ../../../crf/src/cqueue.c
                ../../../crf/src/crf.c
                ../../../crf/src/cbits.c
                )

target_include_directories(spi_master_test PRIVATE
                tinc
                inc
                ../inc
                ../../interfaces/test/inc
                ../../interfaces/inc
                ../../../crf/inc
                )

target_compile_definitions(spi_master_test PRIVATE 
                NDEBUG
                SIGNAL_CLASSES_H=\"${CMAKE_CURRENT_SOURCE_DIR}/inc/signal_classes.h\")

target_compile_options(spi_master_test PRIVATE 
    -Wall 
    -Wextra 
    -pedantic 
    # -Werror
    )
target_link_options(spi_master_test PRIVATE
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map   
)

target_link_libraries(spi_master_test PRIVATE unity)

diagnostic(spi_master_test)

enable_testing()

add_test(NAME spi_master_test
        COMMAND spi_master_test
       )