#ifndef INC_SIGNAL_CLASSES_H
#define INC_SIGNAL_CLASSES_H

#include "crf.h"

typedef enum sig_class_ten {
    SIG_CLASS_SYS = CRF_SIGNAL_CLASS_START,
    SIG_CLASS_MEM,
    SIG_CLASS_CAN,
    SIG_CLASS_CANOPEN
} sig_class_ten;

#endif