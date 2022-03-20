cmake_minimum_required(VERSION 3.19)

add_library(crf_interfaces STATIC 
        src/sys_if.c
)

add_library(chsm::crf_interfaces ALIAS crf_interfaces)

target_compile_definitions(crf_interfaces PUBLIC 
                NDEBUG
                $<CHSM_SIGNAL_CLASSES_H : SIGNAL_CLASSES_H=${CHSM_SIGNAL_CLASSES_H}>
                $<IF:>
                SIGNAL_CLASSES_H=${CHSM_SIGNAL_CLASSES_H}
)

message(STATUS "${}")
if(enable_testing()){
      if(the build target is ltc2471_test)
              target_compile_definitions(crf_interfaces PUBLIC ltc2471_test_sig_class::SIGNAL_CLASSES_H)
      endif()
        
}

target_include_directories(crf_interfaces
PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/crf_interfaces>
)

target_link_libraries(crf_interfaces PUBLIC
    crf
)

install_lib(crf_interfaces chsm 11)

# -------- Test utilities -----------------------

# --------------- i2c test utils ------------------
add_library(i2c_test_utils STATIC 
        test/src/ut_i2c_driver_mock.c
)

add_library(chsm::i2c_test_utils ALIAS i2c_test_utils)

target_compile_definitions(i2c_test_utils PUBLIC 
        NDEBUG
        SIGNAL_CLASSES_H=${CHSM_SIGNAL_CLASSES_H}
)

target_include_directories(i2c_test_utils
PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/test_utils>
)


target_link_libraries(i2c_test_utils PUBLIC
    crf
    crf_interfaces
)

install_lib(i2c_test_utils chsm 11)

# -----------------spi test utils ---------------------

add_library(spi_test_utils STATIC 
        test/src/ut_spi_driver_mock.c
)

add_library(chsm::spi_test_utils ALIAS spi_test_utils)

target_compile_definitions(spi_test_utils PUBLIC 
        NDEBUG
        SIGNAL_CLASSES_H=${CHSM_SIGNAL_CLASSES_H}
)

target_include_directories(spi_test_utils
PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/test_utils>
)


target_link_libraries(spi_test_utils PUBLIC
    crf
    crf_interfaces
)

install_lib(spi_test_utils chsm 11)
