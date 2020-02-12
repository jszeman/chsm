/*
 * chsm_test_functions.c
 *
 *  Created on: 2019. febr. 4.
 *      Author: jszeman
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "chsm.h"
#include "chsm_test.h"

extern char log_str[1024];

uint16_t a(CHsm *this, const CEvent *e)
{
	switch (e->sig)
	{
	case C_SIG_INIT:
		strcat(log_str, "a_init");
		return C_TRAN(a);

	case C_SIG_ENTRY:
		strcat(log_str, "a_entry");
		return C_HANDLED();

	default:
		strcat(log_str, "a_default");
		return C_IGNORED();
	}
}

/*************************************************/

uint16_t b1(CHsm *this, const CEvent *e);
uint16_t b(CHsm *this, const CEvent *e)
{
	switch (e->sig)
	{
	case C_SIG_ENTRY:
		strcat(log_str, "b, ");
		return C_ENTRY(b1);

	default:
		return C_IGNORED();
	}
}


uint16_t b1(CHsm *this, const CEvent *e)
{
	switch (e->sig)
	{
	case C_SIG_ENTRY:
		strcat(log_str, "b1");
		return C_HANDLED();

	default:
		return C_IGNORED();
	}
}

/*************************************************/

/*uint16_t c(CHsm *this, const CEvent *e)
{
	switch (e->sig)
	{
	case C_SIG_ENTRY:
		strcat(log_str, "c, ");
		return C_HANDLED();

	case CHSM_TEST_SIGNAL_1:
		strcat(log_str, "test_sig");
		return C_HANDLED();

	default:
		return C_IGNORED();
	}
}*/

/*************************************************/
