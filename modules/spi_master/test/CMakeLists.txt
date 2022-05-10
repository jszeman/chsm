cmake_minimum_required(VERSION 3.19)

source_abs_path_list(crf_interfaces crf_interfaces_sources)
source_abs_path_list(spi_test_utils spi_test_utils_sources)
source_abs_path_list(spi_master spi_master_sources)

add_executable(spi_master_test 
                tsrc/main.c
                tsrc/ut_spi_master_test.c
                ${crf_interfaces_sources}
                ${spi_test_utils_sources}
                ${spi_master_sources}
                )

target_include_directories(spi_master_test PRIVATE
                tinc
                $<TARGET_PROPERTY:crf_interfaces,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:spi_test_utils,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:spi_master,INCLUDE_DIRECTORIES>
                )

target_compile_definitions(spi_master_test PRIVATE 
                NDEBUG
                SIGNAL_CLASSES_H="${CMAKE_CURRENT_SOURCE_DIR}/tinc/signal_classes.h"
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
                )

diagnostic(spi_master_test)

add_test(NAME spi_master_test
        COMMAND spi_master_test
       )
