cmake_minimum_required(VERSION 3.19)

source_abs_path_list(crf_interfaces crf_interfaces_sources)
source_abs_path_list(i2c_test_utils i2c_test_utils_sources)
source_abs_path_list(i2c_master i2c_master_sources)
source_abs_path_list(lm73 lm73_sources)

add_executable(lm73_test 
        tsrc/main.c
        tsrc/ut_lm73_test.c
        ${crf_interfaces_sources}
        ${i2c_test_utils_sources}
        ${i2c_master_sources}
        ${lm73_sources}
)

target_include_directories(lm73_test PRIVATE
        tinc
        $<TARGET_PROPERTY:crf_interfaces,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:i2c_test_utils,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:i2c_master,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:lm73,INCLUDE_DIRECTORIES>
)

target_compile_definitions(lm73_test PRIVATE 
        NDEBUG
        SIGNAL_CLASSES_H="${CMAKE_CURRENT_SOURCE_DIR}/tinc/signal_classes.h"
)

target_compile_options(lm73_test PRIVATE 
        -Wall 
        -Wextra 
        -pedantic 
)
target_link_options(lm73_test PRIVATE
        -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map   
)

target_link_libraries(lm73_test PRIVATE
        unity
        crf 
        )

diagnostic(lm73_test)

add_test(NAME lm73_test
        COMMAND lm73_test
       )
