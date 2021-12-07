cmake_minimum_required(VERSION 3.19)

set(GLOBAL crf_interfaces_dir ${CMAKE_CURRENT_SOURCE_DIR})

add_library(crf_interfaces INTERFACE 
        src/sys_if.c
)

add_library(chsm::crf_interfaces ALIAS crf_interfaces)

target_compile_definitions(crf_interfaces INTERFACE 
                NDEBUG
)

target_include_directories(crf_interfaces
INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/crf_interfaces>
)

# target_sources(crf_interfaces INTERFACE 
#     ${CMAKE_CURRENT_SOURCE_DIR}/test/inc/signal_classes.h)

target_link_libraries(crf_interfaces INTERFACE
    crf
)

include(diagnostic)
diagnostic(crf_interfaces)

# -------- Test utilities -----------------------

add_library(test_utils INTERFACE 
        test/src/ut_i2c_driver_mock.c
        test/src/ut_spi_driver_mock.c
)

add_library(chsm::test_utils ALIAS test_utils)

target_compile_definitions(test_utils INTERFACE 
        NDEBUG
)

target_include_directories(test_utils
INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/test_utils>
)


target_link_libraries(test_utils INTERFACE
    crf
    crf_interfaces
)

include(diagnostic)
diagnostic(test_utils)
