cmake_minimum_required(VERSION 3.19)

add_library(crf_interfaces STATIC 
        src/sys_if.c
)

add_library(chsm::crf_interfaces ALIAS crf_interfaces)

target_compile_definitions(crf_interfaces PUBLIC 
                NDEBUG
                SIGNAL_CLASSES_H=${CHSM_SIGNAL_CLASSES_H}
)

target_include_directories(crf_interfaces
PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/crf_interfaces>
)

message(STATUS "CHSM_SIGNAL_CLASSES_H: ------------> ${CHSM_SIGNAL_CLASSES_H}")

target_link_libraries(crf_interfaces PUBLIC
    crf
)

diagnostic(crf_interfaces)

# -------- Test utilities -----------------------

add_library(test_utils STATIC 
        test/src/ut_i2c_driver_mock.c
        test/src/ut_spi_driver_mock.c
)

add_library(chsm::test_utils ALIAS test_utils)

target_compile_definitions(test_utils PUBLIC 
        NDEBUG
        SIGNAL_CLASSES_H=${CHSM_SIGNAL_CLASSES_H}
)

target_include_directories(test_utils
PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/test_utils>
)


target_link_libraries(test_utils PUBLIC
    crf
    crf_interfaces
)

diagnostic(test_utils)
