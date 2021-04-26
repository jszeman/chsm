#ifndef INC_SIGNAL_CLASSES_H
#define INC_SIGNAL_CLASSES_H

#include "crf.h"

typedef enum sig_class_ten {
    SIG_CLASS_SYS = CRF_SIGNAL_CLASS_START,
    SIG_CLASS_SPI_DRIVER,
    SIG_CLASS_SPI_MASTER,
    SIG_CLASS_DAC8565,
} sig_class_ten;

#endif