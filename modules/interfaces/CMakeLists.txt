cmake_minimum_required(VERSION 3.19)

add_library(crf_interfaces STATIC 
        src/sys_if.c
)

target_compile_definitions(crf_interfaces PRIVATE 
                NDEBUG
)

target_include_directories(crf_interfaces
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/crf_interfaces>
)

target_sources(crf_interfaces PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/test/inc/signal_classes.h)

target_link_libraries(crf_interfaces PUBLIC
    crf
)

include(diagnostic)
diagnostic(crf_interfaces)

# -------- Test utilities -----------------------

add_library(test_utils STATIC 
        test/src/ut_i2c_driver_mock.c
        test/src/ut_spi_driver_mock.c
)

target_compile_definitions(test_utils PRIVATE 
        NDEBUG
)

target_include_directories(test_utils
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/test_utils>
)

target_link_libraries(test_utils PUBLIC
    crf_interfaces
)

include(diagnostic)
diagnostic(test_utils)
