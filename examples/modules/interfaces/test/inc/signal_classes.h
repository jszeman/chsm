#ifndef INC_SIGNAL_CLASSES_H
#define INC_SIGNAL_CLASSES_H

#include "crf.h"

typedef enum sig_class_ten {
    SIG_CLASS_SYS = CRF_SIGNAL_CLASS_START,
    SIG_CLASS_I2C_DRIVER,
} sig_class_ten;

#endif