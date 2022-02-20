cmake_minimum_required(VERSION 3.19)

add_library(crf STATIC 
        src/cbits.c
        src/chsm.c
        src/cpool.c
        src/cqueue.c
        src/crf.c
        src/cvalue.c
)
add_library(chsm::crf ALIAS crf)

target_compile_definitions(crf PRIVATE 
                NDEBUG
)

target_include_directories(crf
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/crf>
)

target_link_libraries(crf PUBLIC
)

install_lib(crf chsm 11)