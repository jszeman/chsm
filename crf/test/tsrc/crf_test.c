/*
 * canopen_test.c
 *
 *  Created on: 2019. jan. 25.
 *      Author: jszeman
 */

#include <chsm.h>
#include "cevent.h"
#include "cpool.h"
#include "cqueue.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "unity_fixture.h"
#include "chsm_test.h"
#include "bus_driver.h"
#include "events.h"
#include "crf.h"

#include <stdio.h>

TEST_GROUP(crf);

static const cevent_tst event_none = {.sig=C_SIG_NONE, .gc_info=0};

static const cevent_tst event_send_data = {.sig=TEST_SIG_SEND_DATA, .gc_info=0};

#define BUFF1_SIZE 	32
#define BUFF2_SIZE 	256

#define POOL_CNT	2

const cevent_tst  	*events0[8];
bus_driver_tst  	bus_driver;

chsm_tst 			*hsm_ap[] = {(chsm_tst *)&bus_driver, NULL};

uint8_t 			buff1[BUFF1_SIZE];
uint8_t				buff2[BUFF2_SIZE];

cpool_tst			pool_ast[POOL_CNT];

crf_tst				crf;

/* bus_driver::send_data
 * 		Send an event with some arbitrary data 
 */
void send_data(bus_driver_tst *self, const cevent_tst *e_pst)
{
	event_bus_data_tst *e;

	//e = crf.new_event(&crf, )
    printf("\n\nsend_data\n\n");
}


TEST_SETUP(crf)
{
	memset(&bus_driver, 0, sizeof(bus_driver));

	memset(&pool_ast, 0, sizeof(pool_ast));
	memset(&buff1, 0, sizeof(buff1));
	memset(&buff2, 0, sizeof(buff2));

	cpool_init(pool_ast+0, buff1, 8, 4, 0x1000);
	cpool_init(pool_ast+1, buff2, 64, 4, 0x2000);
}

TEST_TEAR_DOWN(crf)
{
}

/* init: 
 * Check that the init function properly fills out the function pointers.
 */
TEST(crf, init)
{
	bool res;
	res = crf_init(&crf, hsm_ap, pool_ast, 2);

	TEST_ASSERT_TRUE(res);
	TEST_ASSERT_NOT_NULL(crf.new_event);
}

/* init: 
 * Check that the init fails, if any of the passed pointers are NULL.
 */
TEST(crf, init_NULL)
{
	bool res;

	res = crf_init(NULL, hsm_ap, pool_ast, 2);
	TEST_ASSERT_FALSE(res);

	res = crf_init(&crf, NULL, pool_ast, 2);
	TEST_ASSERT_FALSE(res);

	res = crf_init(&crf, hsm_ap, NULL, 2);
	TEST_ASSERT_FALSE(res);
}


/* one_small_event: 
 * Create a small event, that fits inside the smaller pool, and check
 * that the returned pointer is inside the buffer we provided for the
 * pool.
 */
TEST(crf, one_small_event)
{
	event_small_tst *e;

	e = (event_small_tst *)crf.new_event(&crf, sizeof(event_small_tst));

	TEST_ASSERT_NOT_NULL(e);
	TEST_ASSERT_GREATER_OR_EQUAL(buff1, (uint8_t *)e);
	TEST_ASSERT_LESS_OR_EQUAL(buff1 + sizeof(buff1) - sizeof(event_small_tst),
		(uint8_t *)e);
}

/* two_small_event: 
 * Create two small event, that fit inside the smaller pool, and check
 * that the returned pointers are not equal and are inside the buffer
 * we provided for the pool.
 */
TEST(crf, two_small_event)
{
	event_small_tst *e1;
	event_small_tst *e2;

	e1 = (event_small_tst *)crf.new_event(&crf, sizeof(event_small_tst));
	e2 = (event_small_tst *)crf.new_event(&crf, sizeof(event_small_tst));

	TEST_ASSERT_NOT_EQUAL(e1, e2);
	TEST_ASSERT_NOT_NULL(e2);
	TEST_ASSERT_GREATER_OR_EQUAL(buff1, (uint8_t *)e2);
	TEST_ASSERT_LESS_OR_EQUAL(buff1 + sizeof(buff1) - sizeof(event_small_tst),
		(uint8_t *)e2);
}

/* too_many_small_event: 
 * Create so many small events, that the framework is forced to allocate
 * the last one from a different pool; 
 */
TEST(crf, too_many_small_event)
{
	event_small_tst *e[5];

	for (int i=0; i<5; i++)
	{
		e[i] = (event_small_tst *)crf.new_event(&crf, sizeof(event_small_tst));
	}

	for (int i=0; i<4; i++)
	{
		TEST_ASSERT_NOT_EQUAL(e[i], e[i+1]);
		TEST_ASSERT_NOT_NULL(e[i]);
		TEST_ASSERT_GREATER_OR_EQUAL(buff1, (uint8_t *)e[i]);
		TEST_ASSERT_LESS_OR_EQUAL(buff1 + sizeof(buff1) - sizeof(event_small_tst),
			(uint8_t *)e[i]);
	}

	TEST_ASSERT_NOT_NULL(e[4]);
	TEST_ASSERT_GREATER_OR_EQUAL(buff2, (uint8_t *)e[4]);
	TEST_ASSERT_LESS_OR_EQUAL(buff2 + sizeof(buff2) - sizeof(event_small_tst),
		(uint8_t *)e[4]);
}

/* too_many_events: 
 * Create so many events that all the slots are allocated from the pools.
 * Check that any further call to new_event results NULL.
 */
TEST(crf, too_many_events)
{
	event_small_tst *e[9];

	for (int i=0; i<9; i++)
	{
		e[i] = (event_small_tst *)crf.new_event(&crf, sizeof(event_small_tst));
	}

	for (int i=0; i<4; i++)
	{
		TEST_ASSERT_NOT_EQUAL(e[i], e[i+1]);
		TEST_ASSERT_NOT_NULL(e[i]);
		TEST_ASSERT_GREATER_OR_EQUAL(buff1, (uint8_t *)e[i]);
		TEST_ASSERT_LESS_OR_EQUAL(buff1 + sizeof(buff1) - sizeof(event_small_tst),
			(uint8_t *)e[i]);
	}

	for (int i=4; i<8; i++)
	{
		TEST_ASSERT_NOT_EQUAL(e[i], e[i+1]);
		TEST_ASSERT_NOT_NULL(e[i]);
		TEST_ASSERT_GREATER_OR_EQUAL(buff2, (uint8_t *)e[i]);
		TEST_ASSERT_LESS_OR_EQUAL(buff2 + sizeof(buff2) - sizeof(event_small_tst),
			(uint8_t *)e[i]);
	}

	TEST_ASSERT_NULL(e[8]);
}



/* garbage_collect: 
 * Create so many events that not all of them will fit in the small pool, then call the
 * GC on an event in the small pool and check that the next new event will be allocated
 * in the slot that the GC freed up.
 */
TEST(crf, garbage_collect)
{
	event_small_tst *e[5];

	for (int i=0; i<5; i++)
	{
		e[i] = (event_small_tst *)crf.new_event(&crf, sizeof(event_small_tst));
	}

	crf.gc(&crf, (cevent_tst *)(e[2]));

	event_small_tst *e_new;

	e_new = (event_small_tst *)crf.new_event(&crf, sizeof(event_small_tst));

	TEST_ASSERT_NOT_NULL(e);
	TEST_ASSERT_GREATER_OR_EQUAL(buff1, (uint8_t *)e_new);
	TEST_ASSERT_LESS_OR_EQUAL(buff1 + sizeof(buff1) - sizeof(event_small_tst),
		(uint8_t *)e_new);
}

/* sratchpad
 */
/*TEST(crf, sandbox)
{
	chsm_ctor((chsm_tst *)&bus_driver, bus_driver_top);
	chsm_init((chsm_tst *)&bus_driver);

	chsm_dispatch((chsm_tst *)&bus_driver, &event_send_data);
}*/

TEST_GROUP_RUNNER(crf)
{
	RUN_TEST_CASE(crf, init);
	RUN_TEST_CASE(crf, init_NULL);
	RUN_TEST_CASE(crf, one_small_event);
	RUN_TEST_CASE(crf, two_small_event);
	RUN_TEST_CASE(crf, too_many_small_event);
	RUN_TEST_CASE(crf, too_many_events);
	RUN_TEST_CASE(crf, garbage_collect);
	//RUN_TEST_CASE(crf, test0);
	//RUN_TEST_CASE(crf, test0);
	//RUN_TEST_CASE(crf, test0);
	//RUN_TEST_CASE(crf, test0);
	//RUN_TEST_CASE(crf, test0);
	//RUN_TEST_CASE(crf, test0);
	//RUN_TEST_CASE(crf, test0);
	//RUN_TEST_CASE(crf, test0);
	//RUN_TEST_CASE(crf, test0);
	//RUN_TEST_CASE(crf, test0);
	//RUN_TEST_CASE(crf, test0);
	//RUN_TEST_CASE(crf, test0);
	//RUN_TEST_CASE(crf, test0);
}
