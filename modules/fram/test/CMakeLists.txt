cmake_minimum_required(VERSION 3.19)

add_executable(fram_test 
                tsrc/main.c
                tsrc/ut_fram_test.c
                )

target_include_directories(fram_test PRIVATE
                tinc
                )

target_compile_definitions(fram_test PRIVATE 
                NDEBUG
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
                crf_interfaces
                i2c_test_utils
                i2c_master
                fram
                # fram_test_sig_class
                )

diagnostic(fram_test)

add_test(NAME fram_test
        COMMAND fram_test
       )

# --------------- Signal_classes handling target -----------------------

target_compile_definitions(fram_test PUBLIC
        SIGNAL_CLASSES_H=${CMAKE_CURRENT_SOURCE_DIR}/tinc/signal_classes.h
)   

target_compile_definitions(crf_interfaces PUBLIC
        SIGNAL_CLASSES_H=${CMAKE_CURRENT_SOURCE_DIR}/tinc/signal_classes.h
)

target_compile_definitions(i2c_master PUBLIC
        SIGNAL_CLASSES_H=${CMAKE_CURRENT_SOURCE_DIR}/tinc/signal_classes.h
)

target_compile_definitions(i2c_test_utils PUBLIC
        SIGNAL_CLASSES_H=${CMAKE_CURRENT_SOURCE_DIR}/tinc/signal_classes.h
)