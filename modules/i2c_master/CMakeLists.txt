cmake_minimum_required(VERSION 3.19)

add_library(i2c_master STATIC 
        src/i2c_master.c
        src/i2c_master_functions.c
)

target_compile_definitions(i2c_master PRIVATE 
        NDEBUG
)

target_include_directories(i2c_master
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/i2c_master>
)

target_link_libraries(i2c_master PUBLIC
crf_interfaces
)

include(diagnostic)
diagnostic(i2c_master)