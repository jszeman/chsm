cmake_minimum_required(VERSION 3.19)

project(CrfTest
        LANGUAGES C)

include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../../cmake/utils")

message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}.")

include(Fetch_unity)

add_executable(crf_test 
                tsrc/main.c
                tsrc/cpool_test.c
                tsrc/cqueue_test.c
                tsrc/chsm_test.c
                tsrc/crf_test.c
                tsrc/cbits_test.c
                tsrc/cvalue_test.c
                tsrc/chsm_test_functions.c
                tsrc/chsm_test_machine.c
                tsrc/chsm_test_machine4.c
                tsrc/bus_driver.c
                tsrc/bus_driver_functions.c
                tsrc/dev_driver.c
                tsrc/dev_driver_functions.c
                tsrc/atomic_functions.c
                ../src/chsm.c
                ../src/cvalue.c
                ../src/cpool.c
                ../src/cqueue.c
                ../src/crf.c
                ../src/cbits.c
                )

target_include_directories(crf_test PRIVATE
                tinc
                ../inc
                )


target_compile_options(crf_test PRIVATE 
                -Wall 
                -Wextra 
                -pedantic
                # -Werror
                )

target_compile_definitions(crf_test PRIVATE
                NDEBUG)

target_link_options(crf_test PRIVATE
    -Wl,-Map=${PROJECT_BINARY_DIR}/bin/${PROJECT_NAME}.map   
)

target_link_libraries(crf_test
                unity
                )
                
include(diagnostic)
set(debug on)
set(autodetect off)
diagnostic(crf_test)
