cmake_minimum_required(VERSION 3.19)

add_library(canopen STATIC 
        src/canopen.c
        src/canopen_functions.c
        src/od.c
        src/sdo.c
        src/sdo_block_dl.c
        src/sdo_block_ul.c
        src/sdo_dl.c
        src/sdo_functions.c
        src/sdo_ul.c
)

add_library(chsm::canopen ALIAS canopen)

target_compile_definitions(canopen PRIVATE 
        NDEBUG
        SIGNAL_CLASSES_H=${CHSM_SIGNAL_CLASSES_H}
)

target_include_directories(canopen
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/canopen>
)

target_link_libraries(canopen PUBLIC
crf_interfaces
)

install_lib(canopen chsm 11)