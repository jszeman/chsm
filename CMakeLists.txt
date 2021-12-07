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

get_target_property(crf_interfaces_dir  crf_interfaces   INCLUDE_DIRECTORIES)
get_target_property(test_utils_dir      test_utils       INCLUDE_DIRECTORIES)
get_target_property(crf_dir             crf              INCLUDE_DIRECTORIES)
get_target_property(i2c_master_dir      i2c_master       INCLUDE_DIRECTORIES)
get_target_property(spi_master_dir      spi_master       INCLUDE_DIRECTORIES)
get_target_property(canopen_dir         canopen          INCLUDE_DIRECTORIES)
get_target_property(dac8565_dir         dac8565          INCLUDE_DIRECTORIES)
get_target_property(fram_dir            fram             INCLUDE_DIRECTORIES)
get_target_property(lm73_dir            lm73             INCLUDE_DIRECTORIES)
get_target_property(ltc2471_dir         ltc2471          INCLUDE_DIRECTORIES)

set(${PROJECT_NAME}_PUBLIC_HEADERS
    ${crf_interfaces_dir}
    ${test_utils}
    ${crf_dir}           
    ${i2c_master_dir}    
    ${spi_master_dir}    
    ${canopen_dir}       
    ${dac8565_dir}       
    ${fram_dir}          
    ${lm73_dir}          
    ${ltc2471_dir}       
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES 
        C_STANDARD          11
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS        OFF
        PUBLIC_HEADER       "${${PROJECT_NAME}_PUBLIC_HEADERS}"
        EXPORT_NAME         ${PROJECT_NAME}
)

# get_target_property(chsm_dir chsm INCLUDE_DIRECTORIES)
# message(FATAL_ERROR "-----> chsm includes: ----> ${${PROJECT_NAME}_PUBLIC_HEADERS}")
get_target_property(i2c_master__dir chsm::i2c_master INCLUDE_DIRECTORIES)
message(FATAL_ERROR "-----> chsm includes: ----> ${i2c_master__dir}")
 
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
                    test_utils
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

message(CHECK_PASS " The targets can now be imported with find_package(${PROJECT_NAME})")
message(STATUS "[${PROJECT_NAME}]: Build the \"install\" target and add \"${CMAKE_INSTALL_PREFIX}/include\" to you PATH for ${PROJECT_NAME} to work")

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
