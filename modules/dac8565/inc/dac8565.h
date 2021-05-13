#ifndef DAC8565_H
#define DAC8565_H

#include "spi_master.h"
#include "crf.h"
#include SIGNAL_CLASSES_H

/*
DAC8565
=======
*/


typedef enum dac8565_signals_ten
{
    SIG_DAC8565_OUTPUTS = SIGNAL_FROM_CLASS(SIG_CLASS_DAC8565),

    /* Result signals */
    SIG_DAC8565_SUCCESS,
    SIG_DAC8565_FAIL,
} dac8565_signals_ten;

#define SIG_DAC8565_OUTPUTS_TYPE    dac8565_outputs_tst

#define SIG_DAC8565_SUCCESS_TYPE    cevent_tst
#define SIG_DAC8565_FAIL_TYPE       cevent_tst

/*
 * EVENT DEFINITIONS
 */

typedef struct dac8565_outputs_tst
{
    cevent_tst          super;          // Signal and GC stuff
    uint16_t            outputs[4];
} dac8565_outputs_tst;

/*
 * ACTIVE OBJECT
 */

typedef struct dac8565_tst dac8565_tst;

struct dac8565_tst
{
    chsm_tst            super;

    spi_transaction_tst trans_ast[4];
    uint32_t            cmd_au32[4];
};

chsm_result_ten dac8565_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);


#endif
