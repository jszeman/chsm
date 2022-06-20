cmake_minimum_required(VERSION 3.19)

add_library(fram STATIC 
    src/fram.c
    src/fram_functions.c
)

add_library(chsm::fram ALIAS fram)

target_compile_definitions(fram PRIVATE 
                NDEBUG
                SIGNAL_CLASSES_H=${CHSM_SIGNAL_CLASSES_H}
)

target_include_directories(fram
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/fram>
)

target_link_libraries(fram PUBLIC
    i2c_master
)

install_lib(fram chsm 11)
