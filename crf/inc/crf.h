/*
 * cr.h
 *
 *  Created on: 2019. jan. 31.
 *      Author: jszeman
 *
 * C Active Object System
 */

#ifndef INC_CRF_H_
#define INC_CRF_H_

#include <stdint.h>
#include <stdbool.h>

#include "cevent.h"
#include "cpool.h"
#include "cqueue.h"
#include "chsm.h"

#define CRF_MAX_POOL_COUNT 4

typedef struct crf_st crf_tst;

struct crf_st
{
	cevent_tst*			(*new_event)(crf_tst *self, uint32_t size);
	void				(*gc)(crf_tst *self, cevent_tst* e);

	chsm_tst			**chsm_ap;		//< Pointer to the array of objects in the application
	cpool_tst			*pool_ast;			//< Pointer to an array of memory pools.
	uint16_t			pool_cnt_u16;
} ;

bool crf_init(crf_tst *self , chsm_tst **chsm_ap, cpool_tst *pool_ast, uint16_t pool_cnt);

#endif /* INC_CRF_H_ */
