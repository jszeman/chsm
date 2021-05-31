cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

# set target system
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION  1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if(MINGW OR CYGWIN OR WIN32)
    set(UTIL_SEARCH_CMD where)
elseif(UNIX OR APPLE)
    set(UTIL_SEARCH_CMD which)
endif()

set(toolchain_name "armcl")
set(CMAKE_C_COMPILER_ID ${toolchain_name})

if(autodetect_toolchain)
  find_program(toolchain ${toolchain_name})
  if(toolchain)
    execute_process(
      COMMAND ${UTIL_SEARCH_CMD} ${toolchain_name}
      OUTPUT_VARIABLE BINUTILS_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --blue --bold "Find tolchain as ${BINUTILS_PATH}")
    get_filename_component(TOOLCHAIN_DIR ${BINUTILS_PATH} DIRECTORY)
    message(STATUS "TOOLCHAIN_DIR: -> ${TOOLCHAIN_DIR}")
    get_filename_component(PARENT_TOOLCHAIN_DIR ${TOOLCHAIN_DIR} DIRECTORY)
    message(STATUS "PARENT_TOOLCHAIN_DIR: -> ${PARENT_TOOLCHAIN_DIR}")
  else()
    execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red --bold "    Didn't find toolchain!
    It's possible you forget to add it to the system PATH variable.")
    message(FATAL_ERROR "")
  endif()
else()
  set(TOOLCHAIN_DIR "C:/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin")
  find_program(toolchain NAMES ${toolchain_name} PATHS ${TOOLCHAIN_DIR} NO_DEFAULT_PATH)
  if(toolchain)
    execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --blue --bold "Find tolchain as ${TOOLCHAIN_DIR}")
    message(STATUS "TOOLCHAIN_DIR: -> ${TOOLCHAIN_DIR}")
    get_filename_component(PARENT_TOOLCHAIN_DIR ${TOOLCHAIN_DIR} DIRECTORY)
    message(STATUS "PARENT_TOOLCHAIN_DIR: -> ${PARENT_TOOLCHAIN_DIR}")
  else()
    execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red --bold "    Didn't find toolchain!
    It's possible you forget to add it to the system PATH variable.")
    message(FATAL_ERROR "")
  endif()
endif()

set(CMAKE_FIND_ROOT_PATH "${TOOLCHAIN_DIR}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# toolchain paths
find_program(TI_GCC             NAMES   armcl    PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TI_CXX             NAMES   armcl    PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TI_AS              NAMES   armcl    PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TI_AR              NAMES   armar    PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TI_OBJCOPY         NAMES   armofd   PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TI_OBJDUMP         NAMES   armhex   PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TI_SIZE            NAMES   armsize  PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TI_LD              NAMES   armcl    PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)

# set executables settings
set(CMAKE_C_COMPILER    ${TI_GCC})
set(CMAKE_CXX_COMPILER  ${TI_CXX})
set(AS                  ${TI_AS})
set(AR                  ${TI_AR})
set(OBJCOPY             ${TI_OBJCOPY})
set(OBJDUMP             ${TI_OBJDUMP})
set(SIZE                ${TI_SIZE})
set(LD                  ${TI_LD})

add_definitions(
  -DOD_EXTENSION
  -DFPGA_PWM
  -D_RADIUS_INTERFACE_ACTIVE
  -DNODEBUG
  -DBOARD_EL_24_02_03
  -D_FLASH
  -DFPGA_TEST)

set(CMAKE_C_STANDARD_COMPUTED_DEFAULT 99)

add_compile_options(
    -mv7M3
    --code_state=16
    -me
    -O2
    --c99
    --gcc
    --diag_warning=225
    --diag_wrap=off
    --display_error_number
    --gen_func_subsections=on
    --abi=eabi
    --ual )

    include_directories("${TOOLCHAIN_DIR}/../include")