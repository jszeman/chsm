
source_abs_path_list(chsm_interfaces chsm_interfaces_sources)

add_module_test(
        NAME
                intf_test
        SOURCE 
                tsrc/main.c
                tsrc/ut_i2c_driver_mock.c
                tsrc/ut_i2c_driver_mock_test.c
                tsrc/ut_spi_driver_mock.c
                tsrc/ut_spi_driver_mock_test.c
                ${chsm_interfaces_sources}
        INCLUDE 
                tinc
                $<TARGET_PROPERTY:chsm_interfaces,INCLUDE_DIRECTORIES>
        LINK 
                unity
                chsm::crf
        DEFINES 
                NDEBUG
                SIGNAL_CLASSES_H="${CMAKE_CURRENT_SOURCE_DIR}/tinc/signal_classes.h"
    )

# -------- Test utilities -----------------------

# --------------- i2c test utils -----------------
add_module_lib(
        NAME 
                i2c_test_utils
        PACKAGE
                chsm
        SOURCE
                tsrc/ut_i2c_driver_mock.c
        INCLUDE 
                tinc
        LINK 
                chsm::chsm_interfaces
                chsm::crf
        DEFINES 
                NDEBUG
                SIGNAL_CLASSES_H="${CHSM_SIGNAL_CLASSES_H}"
        STANDARD
                11
)

# -----------------spi test utils ---------------------
add_module_lib(
        NAME 
                spi_test_utils
        PACKAGE
                chsm
        SOURCE
                tsrc/ut_spi_driver_mock.c
        INCLUDE 
                tinc
        LINK 
                chsm::chsm_interfaces
                chsm::crf
        DEFINES 
                NDEBUG
                SIGNAL_CLASSES_H="${CHSM_SIGNAL_CLASSES_H}"
        STANDARD
                11
)