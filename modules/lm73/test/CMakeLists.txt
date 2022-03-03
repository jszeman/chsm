cmake_minimum_required(VERSION 3.19)

include(Fetch_unity)

# set(SIGNAL_CLASSES_H \"${CMAKE_CURRENT_SOURCE_DIR}/inc/signal_classes.h\")
# set(CHSM_SIGNAL_CLASSES_H ${SIGNAL_CLASSES_H} CACHE INTERNAL "Set CHSM_SIGNAL_CLASSES_H with local path.") 

# message("from lm73_test CHSM_SIGNAL_CLASSES_H : -----------> ${CHSM_SIGNAL_CLASSES_H}")


add_executable(lm73_test 
src/main.c
src/ut_lm73_test.c
../src/lm73.c
../src/lm73_functions.c
)
include(set_signal_classes)
add_custom_command(
        TARGET lm73_test
        PRE_BUILD
        COMMENT "Set the nessesary path for the ${target} based CHSM_SIGNAL_CLASSES_H." 
        COMMAND ${CMAKE_COMMAND} -P set_signal_classes
        )

target_include_directories(lm73_test PRIVATE
        inc
        ../inc
)

target_compile_definitions(lm73_test PRIVATE 
        NDEBUG
        SIGNAL_CLASSES_H=${CHSM_SIGNAL_CLASSES_H}
        
)

target_compile_options(lm73_test PRIVATE 
        -Wall 
        -Wextra 
        -pedantic 
        # -Werror
)
target_link_options(lm73_test PRIVATE
        -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map   
)

target_link_libraries(lm73_test PRIVATE
        unity
        crf 
        crf_interfaces
        i2c_test_utils
        i2c_master
        )

diagnostic(lm73_test)

enable_testing()

add_test(NAME lm73_test
        COMMAND lm73_test
       )