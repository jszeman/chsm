cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

# set target system
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION  1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_USE_RESPONSE_FILE_FOR_LIBRARIES 1)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_LIBRARIES 1)
set(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS 1)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS 1)
set(CMAKE_C_USE_RESPONSE_FILE_FOR_INCLUDES 1)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES 1)
set(CMAKE_C_RESPONSE_FILE_LINK_FLAG "@")
set(CMAKE_CXX_RESPONSE_FILE_LINK_FLAG "@")

if(MINGW OR CYGWIN OR WIN32)
    set(UTIL_SEARCH_CMD where)
elseif(UNIX OR APPLE)
    set(UTIL_SEARCH_CMD which)
endif()

set(toolchain_name "cl2000")
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
  set(TOOLCHAIN_DIR "C:/ti/ccs1020/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS/bin")
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

set(CMAKE_FIND_ROOT_PATH "${TOOLCHAIN_DIR}") # path/bin
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# toolchain paths
find_program(TI_GCC             NAMES   cl2000    PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TI_CXX             NAMES   cl2000    PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TI_AS              NAMES   cl2000    PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TI_AR              NAMES   ar2000    PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TI_OBJCOPY         NAMES   ofd2000   PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TI_OBJDUMP         NAMES   hex2000   PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
# find_program(TI_SIZE            NAMES   size2000  PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)
find_program(TI_LD              NAMES   cl2000    PATHS  ${TOOLCHAIN_DIR}    NO_DEFAULT_PATH)

# set executables settings
set(CMAKE_C_COMPILER    ${TI_GCC})
set(CMAKE_CXX_COMPILER  ${TI_CXX})
set(AS                  ${TI_AS})
set(AR                  ${TI_AR})
set(OBJCOPY             ${TI_OBJCOPY})
set(OBJDUMP             ${TI_OBJDUMP})
# set(SIZE                ${TI_SIZE})
set(LD                  ${TI_LD})

add_definitions(
    -D_INLINE
    -DNO_PROFILING
    -DFPGA_PWM
    -DFPGA_TEST
    # -D__TMS320C2000__
    # -D__TMS320C28X__
    # -D__TMS320C28XX__
    # -D__TMS320C28XX_CLA__
    -DBOARD_EL_24_02_03)

add_compile_options(
    -v28
    -ml
    -mt
    --float_support=fpu32
    --vcu_support=vcu0
    -O2
    --c99
    --diag_warning=225
    --diag_wrap=off
    --display_error_number
    --abi=coffabi )

include_directories("${TOOLCHAIN_DIR}/../include")