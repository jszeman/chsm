cmake_minimum_required(VERSION 3.19)

function(source_abs_path_list target output_source_list)
    get_target_property(sources_list_dir ${target} SOURCE_DIR)
    get_target_property(sources_list ${target} SOURCES)

    foreach(source IN_LISTS sources_list)
        string(PREPEND source "${sources_list_dir}/")
    endforeach()
    set(output_source_list ${source_list})
endfunction(source_abs_path_list target)

source_abs_path_list(crf_interfaces crf_interfaces_sources)
source_abs_path_list(i2c_test_utils i2c_test_utils_sources)
source_abs_path_list(i2c_master i2c_master_sources)
source_abs_path_list(ltc2471 ltc2471_sources)

add_executable(ltc2471_test 
    tsrc/main.c
    tsrc/ut_ltc2471_test.c
    ${crf_interfaces_sources}
    ${i2c_test_utils_sources}
    ${i2c_master_sources}
    ${ltc2471_sources}
)
target_include_directories(ltc2471_test PRIVATE
                tinc
                $<TARGET_PROPERTY:crf_interfaces,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:i2c_test_utils,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:i2c_master,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:ltc2471,INCLUDE_DIRECTORIES>
                )

target_compile_definitions(ltc2471_test PRIVATE 
                NDEBUG
                SIGNAL_CLASSES_H=${CMAKE_CURRENT_SOURCE_DIR}/tinc/signal_classes.h
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
        )

diagnostic(ltc2471_test)

add_test(NAME ltc2471_test
        COMMAND ltc2471_test
       )
