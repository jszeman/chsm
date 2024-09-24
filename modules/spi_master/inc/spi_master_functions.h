#ifndef SPI_MASTER_FUNCTIONS_H
#define SPI_MASTER_FUNCTIONS_H

#include "chsm.h"
#include "cevent.h"
#include "spi_master.h"
#include "stdbool.h"

/* The following functions shall be implemented by the user */

void spi_master_init (chsm_tst *self, const cevent_tst *e_pst);

void spi_master_send_fail (chsm_tst *self, const cevent_tst *e_pst);

void spi_master_send_success (chsm_tst *self, const cevent_tst *e_pst);

void spi_master_start (chsm_tst *self, const cevent_tst *e_pst);



typedef enum spi_master_state_id_ten
{
    S_SPI_MASTER_IDLE = 1,
    S_SPI_MASTER_BUSY = 2,
} spi_master_state_id_ten;

/*
Signals:
    SIG_SPI_DRIVER_TRANSFER_FAIL 
    SIG_SPI_DRIVER_TRANSFER_SUCCESS 
    SIG_SPI_MASTER_TRANSACTION     

The following functions shall be declared and implemented by the user:

*/

#endif