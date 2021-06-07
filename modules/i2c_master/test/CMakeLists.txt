cmake_minimum_required(VERSION 3.19)

project(example_i2c_master_test)

include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}../../../../cmake/utils")

include(Fetch_unity)

add_executable(i2c_master_test 
                src/main.c
                src/ut_i2c_master_test.c
                ../src/i2c_master.c
                ../src/i2c_master_functions.c
                ../../interfaces/test/src/ut_i2c_driver_mock.c
                ../../../crf/src/chsm.c
                ../../../crf/src/cvalue.c
                ../../../crf/src/cpool.c
                ../../../crf/src/cqueue.c
                ../../../crf/src/crf.c
                ../../../crf/src/cbits.c
                )

target_include_directories(i2c_master_test PRIVATE
                tinc
                inc
                ../inc
                ../../interfaces/test/inc
                ../../interfaces/inc
                ../../../crf/inc
                )

target_compile_definitions(i2c_master_test PRIVATE 
                NDEBUG
                SIGNAL_CLASSES_H="signal_classes.h")

target_compile_options(i2c_master_test PRIVATE 
    -Wall 
    -Wextra 
    -pedantic 
    # -Werror
    )
target_link_options(i2c_master_test PRIVATE
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map   
)

target_link_libraries(i2c_master_test PRIVATE unity)

set(debug ON)

include(diagnostic)
diagnostic(i2c_master_test)
