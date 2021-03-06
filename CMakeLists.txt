cmake_minimum_required(VERSION 3.19)

project(crf 
        VERSION 1.0.0
        LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/utils")

add_library(crf   
                crf/src/cbits.c
                crf/src/chsm.c
                crf/src/cpool.c
                crf/src/cqueue.c
                crf/src/crf.c
                crf/src/cvalue.c
                )

add_library(${PROJECT_NAME}::framework ALIAS ${PROJECT_NAME})

target_compile_definitions(crf PRIVATE 
                NDEBUG
)

# Includes ---------------------------------------------------------------------

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)


target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/crf/inc>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/modules/interfaces/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
)

set(${PROJECT_NAME}_PUBLIC_HEADERS
        crf/inc/atomic_ops.h 
        crf/inc/cbits.h      
        crf/inc/cevent.h     
        crf/inc/chsm.h       
        crf/inc/cpool.h      
        crf/inc/cqueue.h     
        crf/inc/crf.h        
        crf/inc/cvalue.h     
    
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES 
        C_STANDARD          99
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS        OFF
        PUBLIC_HEADER       "${${PROJECT_NAME}_PUBLIC_HEADERS}"
        EXPORT_NAME         framework
)

# Install ------------------------------------------------------------------------

write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
    VERSION       ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion 
)

## Target installation
install(TARGETS   ${PROJECT_NAME}
    EXPORT        ${PROJECT_NAME}Targets
    ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
    COMPONENT     library
)

## Target's cmake files: targets export
install(EXPORT  ${PROJECT_NAME}Targets
    NAMESPACE   ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

## Target's cmake files: config and version config for find_package()
install(FILES   ${PROJECT_NAME}Config.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

include(diagnostic)
set(debug on)
# set(autodetect_toolchain on)
diagnostic(crf)