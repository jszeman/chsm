cmake_minimum_required(VERSION 3.19)

add_executable(canopen_test 
                tsrc/main.c
                tsrc/co_nmt_exp_sdo_test.c
                tsrc/co_seg_sdo_test.c
                tsrc/co_block_sdo_test.c
                )

target_include_directories(canopen_test PRIVATE
                tinc
                )

target_compile_definitions(canopen_test PRIVATE 
                NDEBUG
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
                crf_interfaces
                canopen
                # canopen_test_sig_classes
                )

diagnostic(canopen_test)

add_test(NAME canopen_test
        COMMAND canopen_test
       )

# --------------- Signal_classes handling target -----------------------

target_compile_definitions(canopen_test PUBLIC
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