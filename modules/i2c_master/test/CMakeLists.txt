cmake_minimum_required(VERSION 3.19)

source_abs_path_list(crf_interfaces crf_interfaces_sources)
source_abs_path_list(i2c_test_utils i2c_test_utils_sources)
source_abs_path_list(i2c_master i2c_master_sources)

add_executable(i2c_master_test 
                tsrc/main.c
                tsrc/ut_i2c_master_test.c
                ${crf_interfaces_sources}
                ${i2c_test_utils_sources}
                ${i2c_master_sources}
                )

target_include_directories(i2c_master_test PRIVATE
                tinc
                $<TARGET_PROPERTY:crf_interfaces,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:i2c_test_utils,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:i2c_master,INCLUDE_DIRECTORIES>
                )

target_compile_definitions(i2c_master_test PRIVATE 
                NDEBUG
                SIGNAL_CLASSES_H="${CMAKE_CURRENT_SOURCE_DIR}/tinc/signal_classes.h"
                )

target_compile_options(i2c_master_test PRIVATE 
    -Wall 
    -Wextra 
    -pedantic 
    )

target_link_options(i2c_master_test PRIVATE
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map   
)

target_link_libraries(i2c_master_test PRIVATE 
                unity
                crf
                )

diagnostic(i2c_master_test)

add_test(NAME i2c_master_test
        COMMAND i2c_master_test
       )
