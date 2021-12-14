cmake_minimum_required(VERSION 3.19)

add_library(dac8565 STATIC  
    src/dac8565_functions.c
)

add_library(chsm::dac8565 ALIAS dac8565)

target_compile_definitions(dac8565 PRIVATE 
                NDEBUG
                SIGNAL_CLASSES_H=\"${CHSM_SIGNAL_CLASSES_H}\"
)

target_include_directories(dac8565
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/dac8565>
)

target_link_libraries(dac8565 PUBLIC
    i2c_master
)

diagnostic(dac8565)