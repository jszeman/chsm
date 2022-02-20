cmake_minimum_required(VERSION 3.19)

add_library(spi_master STATIC 
        src/spi_master.c
        src/spi_master_functions.c
)

add_library(chsm::spi_master ALIAS spi_master)

target_compile_definitions(spi_master PRIVATE 
        NDEBUG
        SIGNAL_CLASSES_H=\"${CHSM_SIGNAL_CLASSES_H}\"
)

target_include_directories(spi_master
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/spi_master>
)

target_link_libraries(spi_master PUBLIC
crf_interfaces
)

install_lib(spi_master chsm 11)
diagnostic(spi_master)