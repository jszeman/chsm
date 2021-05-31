
include(color_print)

function(get_target_sources target_name locations)
  get_target_property(target_sources ${target_name} SOURCE_DIR )
  set(include_dir "${target_sources}/inc" )
  set(source_dir "${target_sources}/src" )
  string(APPEND locations_ "${include_dir} " "${source_dir} " )
  set(${locations} ${locations_} PARENT_SCOPE)
endfunction()


##################### CLANG_FORMAT ######################

function(scan_source_dirs target_name resource_files)
  get_target_property(target_sources ${target_name} SOURCE_DIR )
  file(
      GLOB_RECURSE
      SOURCE_FILES
      CONFIGURE_DEPENDS
      "${target_sources}/*.[cChH]")
  set(${resource_files} ${SOURCE_FILES} PARENT_SCOPE)
endfunction()

function(clang_format target_name)
  scan_source_dirs(${target_name} res_files)
    find_program(CLANG_FORMAT "clang-format")
    if(CLANG_FORMAT)
    color_print(blue "Format clang-format the ${target_name} .c/.h source files.")
        execute_process(
          # COMMAND clang-format -i -style=file ${res_files}
          COMMAND clang-format -style=file ${res_files}
        )
    else()
        color_print(red  "Can't find clang-format command.")
    endif()
endfunction()


##################### CLANG_TIDY ######################

function(clang_tidy target_name)
  scan_source_dirs(${target_name} res_files)
  find_program(CLANG_TIDY "clang-tidy")
  if(CLANG_TIDY)
    color_print(yellow "Analize clang-tidy the ${target_name} .c/.h source files.")
      execute_process(
      COMMAND clang-tidy ${res_files} )
  else()
      color_print(red  "Can't find clang-format command.")
  endif()
endfunction()


##################### CMAKE_FORMAT ######################

function(cmake_format)
    file(
        GLOB_RECURSE
        ALL_CMAKE_FILES
        CONFIGURE_DEPENDS
        # "cmake/*.cmake"
      "src/**/CMakeLists.txt")
    find_program(CMAKE_FORMAT "cmake-format")
    if(CMAKE_FORMAT)
      color_print(pink "Format cmake-format the CMake files!")
        execute_process(
            # COMMAND cmake-format.exe -i ${ALL_CMAKE_FILES}
          COMMAND cmake-format.exe -i ${ALL_CMAKE_FILES} )
    else()
        color_print(red  "Can't find cmake-format command.")
    endif()
endfunction()
