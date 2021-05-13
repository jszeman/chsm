#ifndef MEM_DRIVER_IF_H
#define MEM_DRIVER_IF_H

#include <stdint.h>
#include <stdbool.h>
#include "crf.h"
#include "cqueue.h"
#include SIGNAL_CLASSES_H
#include "cevent.h"

/*
 * MEMORY INTERFACE SIGNALS
 */

typedef enum data_signals_ten
{
    /* External signals */
    SIG_MEM_READ = SIGNAL_FROM_CLASS(SIG_CLASS_MEM),
    SIG_MEM_WRITE,
    SIG_MEM_WRITE_SUCCESS,
    SIG_MEM_WRITE_FAIL,
    SIG_MEM_READ_SUCCESS,
    SIG_MEM_READ_FAIL
} data_signals_ten;

#define SIG_MEM_READ_TYPE           mem_op_tst
#define SIG_MEM_WRITE_TYPE          mem_op_tst
#define SIG_MEM_WRITE_SUCCESS_TYPE  mem_status_tst
#define SIG_MEM_WRITE_FAIL_TYPE     mem_status_tst
#define SIG_MEM_READ_SUCCESS_TYPE   mem_status_tst
#define SIG_MEM_READ_FAIL_TYPE      mem_status_tst


typedef struct mem_driver_if_tst mem_driver_if_tst;

struct mem_driver_if_tst 
{
    uint32_t    (*read)(mem_driver_if_tst* self, uint8_t* buff_pu8, uint32_t address_u32, uint32_t len_u32, cqueue_tst* q_pst);
    uint32_t    (*write)(mem_driver_if_tst* self, uint8_t* buff_pu8, uint32_t address_u32, uint32_t len_u32, cqueue_tst* q_pst);
};

typedef enum mem_error_ten
{
    MEM_EC_NONE,
    MEM_EC_INVALID_ADDRESS, // The whole or part of the address range is not accessible, not implemented, etc.
    MEM_EC_HW_ACCESS_ERROR, // The memory access failed due to some HW problem (I2C address NACK, for example)
    MEM_EC_READ_ONLY,       // The write operation was targeted at a read only address
} mem_error_ten;

typedef struct mem_op_tst
{
    cevent_tst      super;
    uint32_t        address_u32;
    uint8_t*        buff_pu8;
    uint32_t        len_u32;
    cqueue_tst*     q_pst;
} mem_op_tst;

typedef struct mem_status_tst
{
    cevent_tst      super;
    mem_error_ten   error_en;
    uint32_t        address_u32;
} mem_status_tst;
/*
Motivation
==========

Would be nice to have a common interface to internal RAM and slow external
memories for read and write operations.


Interface description
=====================

The driver implementation shall assume the following:
    * The caller is prepared to keep the buffer available for the whole
      duration of the operation.
    * Parameters are valid, ie no need to check for NULL pointers


read
----
params:
    buff_pu8:       The module shall copy the requested data into this buffer.
    address_u32:    Read start address.
    len_u32:        Number of bytes to read.
    q_pst:          The module shall send events regarding the current operation to this queue.

return:
    0:          The read will be performed asynchronously. The caller shall wait for the response message.
    len_u32:    The read was finished, the data is available in the buffer. The module will not send events to
                the target queue.


write
-----
params:
    buff_pu8:       The module shall copy the requested data from this buffer.
    address_u32:    Write start address.
    len_u32:        Number of bytes to write.
    q_pst:          The module shall send events regarding the current operation to this queue.

return:
    0:          The write will be performed asynchronously. The caller shall wait for the response message.
    len_u32:    The write was finished, the data was copied into the memory. The module will not send events to
                the target queue.
*/

#endif
