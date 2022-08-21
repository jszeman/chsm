cmake_minimum_required(VERSION 3.19)

source_abs_path_list(chsm_interfaces chsm_interfaces_sources)
source_abs_path_list(i2c_test_utils i2c_test_utils_sources)
source_abs_path_list(i2c_master i2c_master_sources)
source_abs_path_list(lm73 lm73_sources)

add_module_test(
        NAME
                lm73_test
        SOURCE 
                tsrc/main.c
                tsrc/ut_lm73_test.c
                ${chsm_interfaces_sources}
                ${i2c_test_utils_sources}
                ${i2c_master_sources}
                ${ltc2471_sources}
        INCLUDE 
                tinc
                $<TARGET_PROPERTY:chsm_interfaces,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:i2c_test_utils,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:i2c_master,INCLUDE_DIRECTORIES>
                $<TARGET_PROPERTY:lm73,INCLUDE_DIRECTORIES>
        LINK 
                unity
                chsm::crf
        DEFINES 
                NDEBUG
                SIGNAL_CLASSES_H="${CMAKE_CURRENT_SOURCE_DIR}/tinc/signal_classes.h"
    )