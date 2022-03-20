cmake_minimum_required(VERSION 3.19)

add_executable(lm73_test 
tsrc/main.c
tsrc/ut_lm73_test.c
)

target_include_directories(lm73_test PRIVATE
        tinc
)

target_compile_definitions(lm73_test PRIVATE 
        NDEBUG
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
        crf_interfaces
        i2c_test_utils
        i2c_master
        lm73
        lm73_test_sig_class
        )

diagnostic(lm73_test)

add_test(NAME lm73_test
        COMMAND lm73_test
       )


# --------------- Signal_classes handling target -----------------------

add_library(lm73_test_sig_class PUBLIC
)

target_include_directories(lm73_test_sig_class PUBLIC
        tinc
        )
target_compile_definitions(lm73_test_sig_classes PUBLIC
        SIGNAL_CLASSES_H=${CMAKE_CURRENT_SOURCE_DIR}/tinc/signal_classes.h
        )   