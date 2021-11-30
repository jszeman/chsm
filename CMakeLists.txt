cmake_minimum_required(VERSION 3.19)

project(chsm 
        VERSION 1.0.0
        LANGUAGES C CXX ASM)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/utils")

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

target_link_libraries(${PROJECT_NAME} INTERFACE
        crf_interfaces
        crf 
        i2c_master 
        spi_master
        canopen 
        dac8565 
        fram 
        lm73 
        ltc2471        
)

# Includes ---------------------------------------------------------------------

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install ------------------------------------------------------------------------

write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
    VERSION       ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion 
)

## Target installation
install(TARGETS   ${PROJECT_NAME} 
                    crf_interfaces 
                    crf
                    i2c_master 
                    spi_master
                    canopen 
                    dac8565 
                    fram 
                    lm73 
                    ltc2471 
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
diagnostic(${PROJECT_NAME})

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
