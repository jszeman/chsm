cmake_minimum_required(VERSION 3.19)

add_executable(ltc2471_test 
    tsrc/main.c
    tsrc/ut_ltc2471_test.c
)

target_include_directories(ltc2471_test PRIVATE
                tinc
                )

target_compile_definitions(ltc2471_test PRIVATE 
                NDEBUG
)

target_compile_options(ltc2471_test PRIVATE 
    -Wall 
    -Wextra 
    -pedantic 
    )

target_link_options(ltc2471_test PRIVATE
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map   
)

target_link_libraries(ltc2471_test PRIVATE 
        unity
        crf 
        crf_interfaces
        i2c_test_utils
        i2c_master
        ltc2471
        ltc2471_test_sig_class
        )

diagnostic(ltc2471_test)

add_test(NAME ltc2471_test
        COMMAND ltc2471_test
       )

# --------------- Signal_classes handling target -----------------------

add_library(ltc2471_test_sig_class PUBLIC
)

target_include_directories(ltc2471_test_sig_class PUBLIC
        tinc
        )
target_compile_definitions(ltc2471_test_sig_classes PUBLIC
        SIGNAL_CLASSES_H=${CMAKE_CURRENT_SOURCE_DIR}/tinc/signal_classes.h
        )   