cmake_minimum_required(VERSION 3.19)

include(Fetch_unity)

# set(SIGNAL_CLASSES_H \"${CMAKE_CURRENT_SOURCE_DIR}/inc/signal_classes.h\")
# set(CHSM_SIGNAL_CLASSES_H ${SIGNAL_CLASSES_H} CACHE INTERNAL "Set CHSM_SIGNAL_CLASSES_H with local path.") 


add_executable(ltc2471_test 
src/main.c
src/ut_ltc2471_test.c
)

include(set_signal_classes)
add_custom_command(
    TARGET ltc2471_test
    PRE_BUILD
    COMMENT "Set the nessesary path for the ltc2471_test based CHSM_SIGNAL_CLASSES_H." 
    COMMAND ${CMAKE_COMMAND} -P set_signal_classes
    VERBATIM 
    )
message("from ltc2471_test CHSM_SIGNAL_CLASSES_H : -----------> ${CHSM_SIGNAL_CLASSES_H}")

target_include_directories(ltc2471_test PRIVATE
                inc
                )

target_compile_definitions(ltc2471_test PRIVATE 
                NDEBUG
                SIGNAL_CLASSES_H=${CHSM_SIGNAL_CLASSES_H}
)

target_compile_options(ltc2471_test PRIVATE 
    -Wall 
    -Wextra 
    -pedantic 
    # -Werror
    )
target_link_options(ltc2471_test PRIVATE
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map   
)

target_link_libraries(ltc2471_test PRIVATE 
        unity
        crf 
        crf_interfaces
        i2c_test_utils
        i2c_master
        ltc2471
        )

set(debug ON)

include(diagnostic)
diagnostic(ltc2471_test)

enable_testing()

add_test(NAME ltc2471_test
        COMMAND ltc2471_test
       )

add_library(ltc2471_test_sig_class
)