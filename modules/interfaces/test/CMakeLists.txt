cmake_minimum_required(VERSION 3.19)

source_abs_path_list(crf_interfaces crf_interfaces_sources)

add_executable(intf_test 
                tsrc/main.c
                tsrc/ut_i2c_driver_mock.c
                tsrc/ut_i2c_driver_mock_test.c
                tsrc/ut_spi_driver_mock.c
                tsrc/ut_spi_driver_mock_test.c
                ${crf_interfaces_sources}
                )

target_include_directories(intf_test PRIVATE
                tinc
                $<TARGET_PROPERTY:crf_interfaces,INCLUDE_DIRECTORIES>
                )

target_compile_definitions(intf_test PRIVATE 
                NDEBUG
                SIGNAL_CLASSES_H="${CMAKE_CURRENT_SOURCE_DIR}/tinc/signal_classes.h"
                )

target_compile_options(intf_test PRIVATE 
    -Wall 
    -Wextra 
    -pedantic 
    )

target_link_options(intf_test PRIVATE
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map   
)

target_link_libraries(intf_test PRIVATE 
                unity
                crf
                )

diagnostic(intf_test)

add_test(NAME intf_test
        COMMAND intf_test
       )

