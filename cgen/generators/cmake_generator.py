import io
import os
import re
import sys

class CmakeGen:
    def __init__(self):
        self.chsm_path = ''

    cmake_library_template = """
cmake_minimum_required(VERSION 3.19)

add_library({name} STATIC 
        src/{name}.c
        src/{name}_functions.c
)

add_library(chsm::{name} ALIAS {name})

target_compile_definitions({name} PUBLIC 
        NDEBUG
        SIGNAL_CLASSES_H=${{CHSM_SIGNAL_CLASSES_H}}
)

target_include_directories({name}
PUBLIC
        $<BUILD_INTERFACE:${{CMAKE_CURRENT_SOURCE_DIR}}/inc>
        $<INSTALL_INTERFACE:${{CMAKE_INSTALL_INCLUDEDIR}}/{name}>
)

target_link_libraries({name} PUBLIC
)

install_lib({name} chsm 11)
diagnostic({name})
"""

    cmake_test_build_template = """
cmake_minimum_required(VERSION 3.19)

list(APPEND CMAKE_MODULE_PATH "${{PROJECT_SOURCE_DIR}}/../../../cmake/utils")

include(Fetch_unity)

add_executable({name}_test 
        src/main.c
        src/ut_{name}_test.c
        ../src/{name}.c
        ../src/{name}_functions.c
)

target_include_directories({name}_test PRIVATE
        inc
        ../inc
)

target_compile_definitions({name}_test PRIVATE 
        NDEBUG
        SIGNAL_CLASSES_H=\"${{CMAKE_CURRENT_SOURCE_DIR}}/inc/signal_classes.h\"
)

target_compile_options({name}_test PRIVATE 
        -Wall 
        -Wextra 
        -pedantic 
        # -Werror
)
target_link_options({name}_test PRIVATE
        -Wl,-Map=${{PROJECT_BINARY_DIR}}/${{PROJECT_NAME}}.map   
)

target_link_libraries({name}_test PRIVATE
        unity
        crf 
        crf_interfaces
        )

enable_testing()

add_test(NAME {name}_test
        COMMAND {name}_test
       )
    """
    cmake_executable_template = """
cmake_minimum_required(VERSION 3.19)

add_executable({name}
    src/main.c
)

target_include_directories({name} PUBLIC
    $<BUILD_INTERFACE:${{CMAKE_CURRENT_SOURCE_DIR}}/inc>
    $<INSTALL_INTERFACE:${{CMAKE_INSTALL_INCLUDEDIR}}/{name}>
)

# set_target_properties({name} PROPERTIES SUFFIX ".out")

target_compile_definitions({name} PUBLIC
    NDEBUG
)

target_link_options({name} PRIVATE
)

target_link_directories({name} PRIVATE)

target_link_libraries({name} PUBLIC
    board
    chsm::crf
)

    """

    def createFile(self, file_loc, file_name, content):
        file_dir = os.path.join(file_loc, file_name)
        try:
            if not os.path.exists(file_dir):
                fd_name = open(file_dir, "a+")
                fd_name.write(content)
                fd_name.close()  
        except OSError:
            pass

    def generate_cmake_files(self, cmake_path, module_name, cmake_target_mode_selector):
        if(cmake_target_mode_selector == "library"):
          self.createFile(os.path.join(str(cmake_path), str(module_name)),
                          "CMakeLists.txt", 
                          self.cmake_library_template.format(name=module_name) )
        elif(cmake_target_mode_selector == "executable"):
          self.createFile(os.path.join(str(cmake_path), str(module_name)),
                          "CMakeLists.txt", 
                          self.cmake_executable_template.format(name=module_name) )
          
        self.createFile(os.path.join(str(cmake_path), str(module_name) + "/test"), 
                        "CMakeLists.txt", 
                        self.cmake_test_build_template.format(name=module_name) )
        
