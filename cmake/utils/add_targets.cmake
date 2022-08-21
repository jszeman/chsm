cmake_minimum_required(VERSION 3.19)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

########################  Static lib target  ############################ 
function(add_module_lib)
    set(options)
    set(args NAME)
    set(list_args PACKAGE SOURCE INCLUDE LINK DEFINES STANDARD)
    cmake_parse_arguments(
        PARSE_ARGV 0
        lib
        "${options}"
        "${args}" 
        "${list_args}"
        )

    add_library(${lib_NAME} STATIC
        ${lib_SOURCE}
    )

    add_library(${lib_PACKAGE}::${lib_NAME} ALIAS ${lib_NAME})

    target_compile_definitions(${lib_NAME} PRIVATE 
        ${lib_DEFINES}
    )

    target_link_libraries(${lib_NAME} PUBLIC
        ${lib_LINK}
    )

    target_include_directories(${lib_NAME}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${lib_INCLUDE}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${lib_NAME}>
    )

    if(EXISTS ${lib_STANDARD})
        set_target_properties(${lib_NAME}
            PROPERTIES 
            C_STANDARD          ${lib_STANDARD}
            C_STANDARD_REQUIRED ON
        )
    else()
        set_target_properties(${lib_NAME}
        PROPERTIES 
        C_STANDARD          99
        C_STANDARD_REQUIRED ON
    )
    endif()

    if(CHSM_BUILD_TESTS)
        enable_testing()
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test)
            add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
        else()
            message(" !!_!!_!! Test directory doesn't exist in ${lib_NAME} library!")
        endif()
    endif()

	diagnostic(${lib_NAME})

endfunction()

########################  Executible target  ############################ 
function(add_module_executable)
set(options)
    set(args NAME)
    set(list_args SOURCE INCLUDE LINK DEFINES STANDARD)
    cmake_parse_arguments(
        PARSE_ARGV 0
        exec
        "${options}"
        "${args}" 
        "${list_args}"
        )

    add_executable(${exec_NAME} STATIC
        ${exec_SOURCE}
    )

    target_compile_definitions(${exec_NAME} PRIVATE 
        ${exec_DEFINES}
    )

    target_link_options(${exec_NAME} PRIVATE
    -Wl,-Map=${PROJECT_BINARY_DIR}/${exec_NAME}.map   
    )

    target_link_libraries(${exec_NAME} PUBLIC
        ${exec_LINK}
    )

    target_include_directories(${exec_NAME}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${exec_INCLUDE}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${exec_NAME}>
    )

    if(EXISTS ${exec_STANDARD})
        set_target_properties(${exec_NAME}
            PROPERTIES 
            C_STANDARD          ${exec_STANDARD}
            C_STANDARD_REQUIRED ON
        )
    else()
        set_target_properties(${exec_NAME}
        PROPERTIES 
        C_STANDARD          99
        C_STANDARD_REQUIRED ON
    )
    endif()

    if(CHSM_BUILD_TESTS)
        enable_testing()
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test)
            add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
        else()
            message(" !!_!!_!! Test directory doesn't exist in ${exec_NAME} executable!")
        endif()
    endif()

	diagnostic(${lib_NAME})

endfunction()

########################  Interface lib target ############################ 
function(add_module_interface_lib)
    set(options)
    set(args NAME)
    set(list_args PACKAGE INCLUDE LINK LINK_DIR DEFINES STANDARD)
    cmake_parse_arguments(
        PARSE_ARGV 0
        lib
        "${options}"
        "${args}" 
        "${list_args}"
        ) 

    add_library(${lib_NAME} INTERFACE)

    add_library(${lib_PACKAGE}::${lib_NAME} ALIAS ${lib_NAME})

    target_link_libraries(${lib_NAME} INTERFACE ${lib_LINK})

    target_link_directories(${lib_NAME} INTERFACE ${lib_LINK_DIR})

    if(EXISTS ${lib_STANDARD})
        set_target_properties(${lib_NAME}
            PROPERTIES 
            C_STANDARD          ${lib_STANDARD}
            C_STANDARD_REQUIRED ON
        )
    else()
        set_target_properties(${lib_NAME}
        PROPERTIES 
        C_STANDARD          99
        C_STANDARD_REQUIRED ON
    )
    endif()

    target_include_directories(${lib_NAME} INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${lib_INCLUDE}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${lib_NAME}>
    )

    diagnostic(${lib_NAME})
endfunction()

########################  Test target  ############################ 
function(add_module_test)
    set(options)
    set(args NAME)
    set(list_args SOURCE INCLUDE LINK DEFINES STANDARD)
    cmake_parse_arguments(
        PARSE_ARGV 0
        test
        "${options}"
        "${args}" 
        "${list_args}"
        )

    add_executable(${test_NAME} STATIC
        ${test_SOURCE}
    )

    target_compile_definitions(${test_NAME} PRIVATE 
        ${test_DEFINES}
    )

    target_link_options(${test_NAME} PRIVATE
    -Wl,-Map=${PROJECT_BINARY_DIR}/${test_NAME}.map   
    )

    target_link_libraries(${test_NAME} PUBLIC
        ${test_LINK}
    )

    target_include_directories(${test_NAME}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${test_INCLUDE}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${test_NAME}>
    )

    if(EXISTS ${lib_STANDARD})
        set_target_properties(${test_NAME}
            PROPERTIES 
            C_STANDARD          ${test_STANDARD}
            C_STANDARD_REQUIRED ON
        )
    else()
        set_target_properties(${test_NAME}
        PROPERTIES 
        C_STANDARD          99
        C_STANDARD_REQUIRED ON
    )
    endif()

    add_test(
        NAME ${test_NAME}
        COMMAND ${test_NAME})

	diagnostic(${test_NAME})
endfunction()