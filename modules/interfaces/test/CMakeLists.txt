cmake_minimum_required(VERSION 3.19)

project(example_intf_test)

include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

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
                SIGNAL_CLASSES_H="signal_classes.h")

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
