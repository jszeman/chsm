
source_abs_path_list(chsm_interfaces chsm_interfaces_sources)
source_abs_path_list(spi_test_utils spi_test_utils_sources)
source_abs_path_list(spi_master spi_master_sources)

add_module_test(
    NAME
        spi_master_test
    SOURCE 
        tsrc/main.c
        tsrc/ut_spi_master_test.c
        ${chsm_interfaces_sources}
        ${spi_test_utils_sources}
        ${spi_master_sources}
    INCLUDE 
        tinc
        $<TARGET_PROPERTY:chsm_interfaces,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:spi_test_utils,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:spi_master,INCLUDE_DIRECTORIES>
    LINK 
        unity
        chsm::crf
    DEFINES 
        NDEBUG
        SIGNAL_CLASSES_H="${CMAKE_CURRENT_SOURCE_DIR}/tinc/signal_classes.h"
    STANDARD
)