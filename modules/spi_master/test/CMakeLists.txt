cmake_minimum_required(VERSION 3.19)

add_executable(spi_master_test 
                tsrc/main.c
                tsrc/ut_spi_master_test.c
                )

target_include_directories(spi_master_test PRIVATE
                tinc
                )

target_compile_definitions(spi_master_test PRIVATE 
                NDEBUG
                )

target_compile_options(spi_master_test PRIVATE 
    -Wall 
    -Wextra 
    -pedantic 
    )

target_link_options(spi_master_test PRIVATE
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map   
)

target_link_libraries(spi_master_test PRIVATE 
                unity
                crf
                crf_interfaces
                spi_test_utils
                spi_master
                # spi_master_test_sig_class
                )

diagnostic(spi_master_test)

add_test(NAME spi_master_test
        COMMAND spi_master_test
       )

# --------------- Signal_classes handling target -----------------------

target_compile_definitions(spi_master_test PUBLIC
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