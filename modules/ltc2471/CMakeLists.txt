cmake_minimum_required(VERSION 3.19)

add_library(ltc2471 STATIC 
        src/ltc2471.c
        src/ltc2471_functions.c
)

add_library(chsm::ltc2471 ALIAS ltc2471)

target_compile_definitions(ltc2471 PRIVATE 
        NDEBUG
        SIGNAL_CLASSES_H=\"${CHSM_SIGNAL_CLASSES_H}\"
)

target_include_directories(ltc2471
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/ltc2471>
)

target_link_libraries(ltc2471 PUBLIC
        i2c_master
)

install_lib(ltc2471 chsm 11)