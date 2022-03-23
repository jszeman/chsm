cmake_minimum_required(VERSION 3.19)

macro(source_abs_path_list target output_source_list)
    get_target_property(sources_list_dir ${target} SOURCE_DIR)
    message(STATUS "sources_list_dir: ---> ${sources_list_dir}")
    get_target_property(sources_list ${target} SOURCES)
    message(STATUS "sources_list: ------>${sources_list}")

    foreach(source ${sources_list})
        string(PREPEND source "${sources_list_dir}/")
        string(APPEND ${output_source_list} "${source};")
        message(STATUS "source: ------> ${source}")
    endforeach()
    message(STATUS "${target} output_source_list: --------> ${output_source_list}")
endmacro()
    
    source_abs_path_list(crf_interfaces crf_interfaces_sources)
    message(STATUS "crf_interfaces output_source_list: --------> ${crf_interfaces_sources}")
    source_abs_path_list(i2c_test_utils i2c_test_utils_sources)
    message(STATUS "i2c_test_utils output_source_list: --------> ${i2c_test_utils_sources}")
    source_abs_path_list(i2c_master i2c_master_sources)
    message(STATUS "i2c_master output_source_list: --------> ${i2c_master_sources}")
    source_abs_path_list(ltc2471 ltc2471_sources)
    message(STATUS "ltc2471 output_source_list: --------> ${ltc2471_sources}")

add_executable(ltc2471_test 
    tsrc/main.c
    tsrc/ut_ltc2471_test.c
    ${crf_interfaces_sources}
    ${i2c_test_utils_sources}
    ${i2c_master_sources}
    ${ltc2471_sources}
)
target_include_directories(ltc2471_test PUBLIC
                tinc
                $<TARGET_PROPERTY:crf_interfaces,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:i2c_test_utils,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:i2c_master,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:ltc2471,INCLUDE_DIRECTORIES>
                )

target_compile_definitions(ltc2471_test PUBLIC 
                NDEBUG
                SIGNAL_CLASSES_H="${CMAKE_CURRENT_SOURCE_DIR}/tinc/signal_classes.h"
)

target_compile_options(ltc2471_test PUBLIC 
    -Wall 
    -Wextra 
    -pedantic 
    )

target_link_options(ltc2471_test PUBLIC
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map   
)

target_link_libraries(ltc2471_test PUBLIC 
        unity
        crf 
        )

diagnostic(ltc2471_test)

add_test(NAME ltc2471_test
        COMMAND ltc2471_test
       )
