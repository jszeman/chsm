cmake_minimum_required(VERSION 3.19)

project(canopen_test)

include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}../../../../cmake/utils")

include(Fetch_unity)

# add_subdirectory(../../../crf ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR} )

add_executable(canopen_test 
                src/main.c
                src/co_nmt_exp_sdo_test.c
                src/co_seg_sdo_test.c
                src/co_block_sdo_test.c
                ../src/canopen.c
                ../src/canopen_functions.c
                ../src/sdo.c
                ../src/sdo_functions.c
                ../src/od.c
                ../src/sdo_block_dl.c
                ../src/sdo_block_ul.c
                ../src/sdo_ul.c
                ../src/sdo_dl.c
                ../../interfaces/src/sys_if.c
                ../../../crf/src/chsm.c
                ../../../crf/src/cvalue.c
                ../../../crf/src/cpool.c
                ../../../crf/src/cqueue.c
                ../../../crf/src/crf.c
                ../../../crf/src/cbits.c
                )

target_include_directories(canopen_test PRIVATE
                tinc
                inc
                ../inc
                ../../interfaces/inc
                ../../../crf/inc
                )

target_compile_definitions(canopen_test PRIVATE 
                NDEBUG
                SIGNAL_CLASSES_H="signal_classes.h")

target_compile_options(canopen_test PRIVATE 
    -Wall 
    -Wextra 
    -pedantic
    -g
    # -Werror
    )
target_link_options(canopen_test PRIVATE
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map   
)

target_link_libraries(canopen_test PRIVATE 
                unity 
                # crf
                )

set(debug ON)

include(diagnostic)
diagnostic(canopen_test)
