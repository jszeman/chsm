cmake_minimum_required(VERSION 3.19)

add_executable(dac8565_test 
                src/main.c
                src/dac8565_test.c
                )

target_include_directories(dac8565_test PRIVATE
                tinc
                )

target_compile_definitions(dac8565_test PRIVATE 
                NDEBUG
                )

target_compile_options(dac8565_test PRIVATE 
    -Wall 
    -Wextra 
    -pedantic 
    )

target_link_options(dac8565_test PRIVATE
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map   
)

target_link_libraries(dac8565_test PRIVATE 
                unity
                crf
                crf_interfaces
                spi_test_utils
                spi_master
                dac8565
                # dac8565_test_sig_class
                )

diagnostic(dac8565_test)

add_test(NAME dac8565_test
        COMMAND dac8565_test
       )

# --------------- Signal_classes handling target -----------------------

target_compile_definitions(dac8565_test PUBLIC
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