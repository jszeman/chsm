
add_module_lib(
        NAME
                chsm_interfaces
        PACKAGE 
                chsm
        SOURCES
                src/sys_if.c
        INCLUDE 
                inc
        LINK 
                chsm::crf
        DEFINES 
                NDEBUG
                SIGNAL_CLASSES_H=${CHSM_SIGNAL_CLASSES_H}
        STANDARD
                11
)

