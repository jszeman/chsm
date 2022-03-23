cmake_minimum_required(VERSION 3.19)

source_abs_path_list(crf_interfaces crf_interfaces_sources)
source_abs_path_list(canopen canopen_sources)

add_executable(canopen_test 
                tsrc/main.c
                tsrc/co_nmt_exp_sdo_test.c
                tsrc/co_seg_sdo_test.c
                tsrc/co_block_sdo_test.c
                ${crf_interfaces_sources}
                ${canopen_sources}
                )

target_include_directories(canopen_test PRIVATE
                tinc
                $<TARGET_PROPERTY:crf_interfaces,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:canopen,INCLUDE_DIRECTORIES>
                )

target_compile_definitions(canopen_test PRIVATE 
                NDEBUG
                SIGNAL_CLASSES_H="${CMAKE_CURRENT_SOURCE_DIR}/tinc/signal_classes.h"
                )

target_compile_options(canopen_test PRIVATE 
    -Wall 
    -Wextra 
    -pedantic 
    )

target_link_options(canopen_test PRIVATE
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map   
)

target_link_libraries(canopen_test PRIVATE 
                unity 
                crf
                )

diagnostic(canopen_test)

add_test(NAME canopen_test
        COMMAND canopen_test
       )
