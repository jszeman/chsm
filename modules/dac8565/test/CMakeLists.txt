cmake_minimum_required(VERSION 3.19)

source_abs_path_list(crf_interfaces crf_interfaces_sources)
source_abs_path_list(spi_test_utils spi_test_utils_sources)
source_abs_path_list(spi_master spi_master_sources)
source_abs_path_list(dac8565 dac8565_sources)

add_executable(dac8565_test 
                src/main.c
                src/dac8565_test.c
                ${crf_interfaces_sources}
                ${spi_test_utils_sources}
                ${spi_master_sources}
                ${dac8565_sources}
                )

target_include_directories(dac8565_test PRIVATE
                tinc
                $<TARGET_PROPERTY:crf_interfaces,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:spi_test_utils,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:spi_master,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:dac8565,INCLUDE_DIRECTORIES>
                )

target_compile_definitions(dac8565_test PRIVATE 
                NDEBUG
                SIGNAL_CLASSES_H="${CMAKE_CURRENT_SOURCE_DIR}/tinc/signal_classes.h"
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
                )

diagnostic(dac8565_test)

add_test(NAME dac8565_test
        COMMAND dac8565_test
       )
