cmake_minimum_required(VERSION 3.19)

include(Fetch_unity)

# set(SIGNAL_CLASSES_H \"${CMAKE_CURRENT_SOURCE_DIR}/inc/signal_classes.h\")
# set(CHSM_SIGNAL_CLASSES_H ${SIGNAL_CLASSES_H} CACHE INTERNAL "Set CHSM_SIGNAL_CLASSES_H with local path.") 

message("from canopen_test CHSM_SIGNAL_CLASSES_H : -----------> ${CHSM_SIGNAL_CLASSES_H}")


add_executable(canopen_test 
                src/main.c
                src/co_nmt_exp_sdo_test.c
                src/co_seg_sdo_test.c
                src/co_block_sdo_test.c
                ../src/canopen.c
                ../src/canopen_functions.c
                ../src/sdo.c
                ../src/sdo_functions.c
                ../src/od.c
                ../src/sdo_block_dl.c
                ../src/sdo_block_ul.c
                ../src/sdo_ul.c
                ../src/sdo_dl.c
                # ../../interfaces/src/sys_if.c
                # ../../../crf/src/chsm.c
                # ../../../crf/src/cvalue.c
                # ../../../crf/src/cpool.c
                # ../../../crf/src/cqueue.c
                # ../../../crf/src/crf.c
                # ../../../crf/src/cbits.c
                )

include(set_signal_classes)
add_custom_command(
    TARGET canopen_test
    PRE_BUILD
    COMMENT "Set the nessesary path for the canopen_test based CHSM_SIGNAL_CLASSES_H." 
    COMMAND ${CMAKE_COMMAND} -P set_signal_classes
    )

                
target_include_directories(canopen_test PRIVATE
                inc
                ../inc
                # ../../interfaces/inc
                # ../../../crf/inc
                )

target_compile_definitions(canopen_test PRIVATE 
                NDEBUG
                SIGNAL_CLASSES_H=${CHSM_SIGNAL_CLASSES_H}
                )

target_compile_options(canopen_test PRIVATE 
    -Wall 
    -Wextra 
    -pedantic 
    # -Werror
    )
target_link_options(canopen_test PRIVATE
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map   
)

target_link_libraries(canopen_test PRIVATE 
                unity 
                # crf
                # crf_interfaces
                )

set(debug ON)

include(diagnostic)
diagnostic(canopen_test)

enable_testing()

add_test(NAME canopen_test
        COMMAND canopen_test
       )