cmake_minimum_required(VERSION 3.19)

add_library(lm73 STATIC 
        src/lm73.c
        src/lm73_functions.c
)

add_library(chsm::lm73 ALIAS lm73)

target_compile_definitions(lm73 PUBLIC 
        NDEBUG
        SIGNAL_CLASSES_H=${CHSM_SIGNAL_CLASSES_H}
)

target_include_directories(lm73
PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/lm73>
)

target_link_libraries(lm73 PUBLIC
        i2c_master
)

install_lib(lm73 chsm 11)
diagnostic(lm73)