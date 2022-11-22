#ifndef SPI_MASTER_FUNCTIONS_H
#define SPI_MASTER_FUNCTIONS_H

/*Generated with CHSM v0.0.0 at 2022.11.22 13.30.18*/

#include "spi_master.h"
#include "chsm.h"
#include "cevent.h"
#include <stdbool.h>


void spi_master_init(chsm_tst *self, const cevent_tst *e_pst);

void spi_master_send_fail(chsm_tst *self, const cevent_tst *e_pst);

void spi_master_send_success(chsm_tst *self, const cevent_tst *e_pst);

void spi_master_start(chsm_tst *self, const cevent_tst *e_pst);


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
*/

/*
Other function notes:
*/
#endif
