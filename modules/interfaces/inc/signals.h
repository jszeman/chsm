#ifndef EVENTS_H
#define EVENTS_H

#include "crf.h"

#ifndef SIGNAL_CLASSES_H

typedef enum sig_class_ten {
    SIG_CLASS_SYS = CRF_SIGNAL_CLASS_START,
    SIG_CLASS_I2C_DRIVER,
    SIG_CLASS_I2C_MASTER,
    SIG_CLASS_UART,
    SIG_CLASS_LM73,
    SIG_CLASS_MEM,
    SIG_CLASS_CAN,
    SIG_CLASS_CANOPEN,
    SIG_CLASS_SPI_DRIVER,
    SIG_CLASS_SPI_MASTER,
    SIG_CLASS_DAC8565
} event_class_ten;

#endif

#endif

