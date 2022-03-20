cmake_minimum_required(VERSION 3.19)

add_executable(i2c_master_test 
                tsrc/main.c
                tsrc/ut_i2c_master_test.c
                )

target_include_directories(i2c_master_test PRIVATE
                tinc
                )

target_compile_definitions(i2c_master_test PRIVATE 
                NDEBUG
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
                crf_interfaces
                i2c_test_utils
                i2c_master
                i2c_master_test_sig_class)

diagnostic(i2c_master_test)

add_test(NAME i2c_master_test
        COMMAND i2c_master_test
       )

# --------------- Signal_classes handling target -----------------------

add_library(i2c_master_test_sig_class PUBLIC
)

target_include_directories(i2c_master_test_sig_class PUBLIC
        tinc
        )
target_compile_definitions(i2c_master_test_sig_class PUBLIC
        SIGNAL_CLASSES_H=${CMAKE_CURRENT_SOURCE_DIR}/tinc/signal_classes.h
        )   