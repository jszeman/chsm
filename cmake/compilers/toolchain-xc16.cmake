cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

# set target system
set(CMAKE_SYSTEM_NAME dsPIC33)
set(CMAKE_SYSTEM_VERSION  1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if(MINGW OR CYGWIN OR WIN32)
    set(UTIL_SEARCH_CMD where)
elseif(UNIX OR APPLE)
    set(UTIL_SEARCH_CMD which)
endif()

set(MCU_MODEL 33FJ128MC802 CACHE STRING "MCU model number set.")

set(toolchain_name "xc16")
set(toolchain_prefix ${toolchain_name})
set(CMAKE_C_COMPILER_ID ${toolchain_name})

find_program(toolchain ${toolchain_name}-gcc)
if(toolchain)
  execute_process(
    COMMAND ${UTIL_SEARCH_CMD} ${toolchain_name}-gcc
    OUTPUT_VARIABLE BINUTILS_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --blue --bold "Find tolchain as ${BINUTILS_PATH}")
else()
  execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red --bold "    Didn't find toolchain!
      It's possible you forget to add it to the system PATH variable.")
    message(FATAL_ERROR "")
endif()

get_filename_component(TOOLCHAIN_DIR ${BINUTILS_PATH} DIRECTORY)
message(STATUS "TOOLCHAIN_DIR: -> ${TOOLCHAIN_DIR}")
get_filename_component(PARENT_TOOLCHAIN_DIR ${TOOLCHAIN_DIR} DIRECTORY)
message(STATUS "PARENT_TOOLCHAIN_DIR: -> ${PARENT_TOOLCHAIN_DIR}")

set(CMAKE_FIND_ROOT_PATH "${TOOLCHAIN_DIR}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# toolchain paths
find_program(TOOLCHAIN_GCC             NAMES   ${toolchain_prefix}-gcc      PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TOOLCHAIN_CXX             NAMES   ${toolchain_prefix}-gcc      PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TOOLCHAIN_AS              NAMES   ${toolchain_prefix}-as       PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TOOLCHAIN_AR              NAMES   ${toolchain_prefix}-ar       PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TOOLCHAIN_OBJCOPY         NAMES   ${toolchain_prefix}-objcopy  PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TOOLCHAIN_OBJDUMP         NAMES   ${toolchain_prefix}-objdump  PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TOOLCHAIN_SIZE            NAMES   ${toolchain_prefix}-bin2hex  PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TOOLCHAIN_LD              NAMES   ${toolchain_prefix}-ld       PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)

# set executables settings
set(CMAKE_C_COMPILER    ${TOOLCHAIN_GCC})
set(CMAKE_CXX_COMPILER  ${TOOLCHAIN_CXX})
set(AS                  ${TOOLCHAIN_AS})
set(AR                  ${TOOLCHAIN_AR})
set(OBJCOPY             ${TOOLCHAIN_OBJCOPY})
set(OBJDUMP             ${TOOLCHAIN_OBJDUMP})
set(SIZE                ${TOOLCHAIN_SIZE})
set(LD                  ${TOOLCHAIN_LD})

# set the default C standard manually this is required by `Compiler/Gnu-C`
set(CMAKE_C_STANDARD_COMPUTED_DEFAULT 99)

add_definitions(
  # -DdsPIC33FJ128MC802
  # -DdsPIC33F
  # -D__dsPIC33F__
  # -D__dsPIC33FJ128MC802__
)

add_compile_options(
  -c
  -mcpu=${MCU_MODEL}
  -O0
  -omf=elf
  -g
  -mlarge-code
  -mlarge-data
  -mno-eds-warn
  -no-legacy-libc
  -MMD
  -MF
  -msmart-io=1
  -Wall
  -msfr-warn=off)
