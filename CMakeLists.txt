cmake_minimum_required(VERSION 3.19)

project(chsm 
VERSION 1.0.0
LANGUAGES C CXX ASM)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/utils")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(diagnostic)
include(install_lib)
include(graphviz)
include(path_list)

set(CMAKE_DEBUG_MSG OFF)
set(CHSM_BUILD_TESTS OFF)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})  

message(STATUS "
----------------------> Chsm <-----------------------------------
")

add_subdirectory(modules/canopen)
add_subdirectory(modules/dac8565)
add_subdirectory(modules/fram)
add_subdirectory(modules/i2c_master)
add_subdirectory(modules/interfaces)
add_subdirectory(modules/lm73)
add_subdirectory(modules/ltc2471)
add_subdirectory(modules/spi_master)
add_subdirectory(crf)

add_library(${PROJECT_NAME} INTERFACE)

set_target_properties(${PROJECT_NAME}
    PROPERTIES 
        C_STANDARD          11
        C_STANDARD_REQUIRED ON
)

target_link_libraries(${PROJECT_NAME} INTERFACE
        crf_interfaces
        test_utils
        crf 
        i2c_master 
        spi_master
        canopen 
        dac8565 
        fram 
        lm73 
        ltc2471        
        )
        
if(CHSM_BUILD_TESTS)
    include(FindUnity)
    message(STATUS "----------> Enabled the tests!")

    enable_testing()
    add_subdirectory(modules/canopen/test)
    # add_subdirectory(modules/dac8565/test)
    add_subdirectory(modules/fram/test)
    add_subdirectory(modules/i2c_master/test)
    add_subdirectory(modules/interfaces/test)
    add_subdirectory(modules/lm73/test)
    add_subdirectory(modules/ltc2471/test)
    add_subdirectory(modules/spi_master/test)
    add_subdirectory(crf/test)
endif()
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

# Installing headers
install(
    DIRECTORY "${${PROJECT_NAME}_PUBLIC_HEADERS}"
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include
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

set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(PACKAGE ${PROJECT_NAME})

# ------------------ Test and diagnostic ----------------------------------------------------

diagnostic(${PROJECT_NAME})
