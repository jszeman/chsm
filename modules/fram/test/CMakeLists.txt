cmake_minimum_required(VERSION 3.19)

source_abs_path_list(crf_interfaces crf_interfaces_sources)
source_abs_path_list(i2c_test_utils i2c_test_utils_sources)
source_abs_path_list(i2c_master i2c_master_sources)
source_abs_path_list(fram fram_sources)

add_executable(fram_test 
                tsrc/main.c
                tsrc/ut_fram_test.c
                ${crf_interfaces_sources}
                ${i2c_test_utils_sources}
                ${i2c_master_sources}
                ${fram_sources}
                )

target_include_directories(fram_test PRIVATE
                tinc
                $<TARGET_PROPERTY:crf_interfaces,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:i2c_test_utils,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:i2c_master,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:fram,INCLUDE_DIRECTORIES>
                )

target_compile_definitions(fram_test PRIVATE 
                NDEBUG
                SIGNAL_CLASSES_H="${CMAKE_CURRENT_SOURCE_DIR}/tinc/signal_classes.h"
                )

target_compile_options(fram_test PRIVATE 
                -Wall 
                -Wextra 
                -pedantic 
                )

target_link_options(fram_test PRIVATE
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map   
)

target_link_libraries(fram_test PRIVATE 
                unity
                crf
                )

diagnostic(fram_test)

add_test(NAME fram_test
        COMMAND fram_test
       )
