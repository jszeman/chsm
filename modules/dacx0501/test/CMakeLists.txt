cmake_minimum_required(VERSION 3.19)

project(example_dacx0501_test)

include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../../../cmake/utils")

include(Fetch_unity)

add_executable(dacx0501_test 
                src/main.c
                src/ut_dacx0501_test.c
                ../src/dacx0501.c
                ../src/dacx0501_functions.c
                ../../interfaces/src/sys_if.c
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

target_include_directories(dacx0501_test PRIVATE
                inc
                ../inc
                ../../i2c_master/inc
                ../../interfaces/test/inc
                ../../interfaces/inc
                ../../../crf/inc
                )

target_compile_definitions(dacx0501_test PRIVATE 
                NDEBUG
                SIGNAL_CLASSES_H="signal_classes.h")

target_compile_options(dacx0501_test PRIVATE 
    -Wall 
    -Wextra 
    -pedantic 
    # -Werror
    )
target_link_options(dacx0501_test PRIVATE
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map   
)

target_link_libraries(dacx0501_test PRIVATE unity)

set(debug ON)

include(diagnostic)
diagnostic(dacx0501_test)
