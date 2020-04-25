/*
 * canopen_test.c
 *
 *  Created on: 2019. jan. 25.
 *      Author: jszeman
 */

#include <cqueue.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "unity_fixture.h"

TEST_GROUP(eq);

cqueue_tst 	eq;
cevent_tst  	*events[8];

TEST_SETUP(eq)
{
	for (int i=0; i<8; i++)
	{
		events[i] = NULL;
	}
}

TEST_TEAR_DOWN(eq)
{
}

/**
 * Test that we can get back one event from the queue
 */
TEST(eq, put_1_get_1)
{
	cevent_tst  e;
	const cevent_tst  *ep;
	int32_t r;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	r = cqueue_put(&eq, &e);
	ep = cqueue_get(&eq);

	TEST_ASSERT_EQUAL(0, r);
	TEST_ASSERT(ep == &e);
}

/**
 * Test that we can get back the first event from the queue,
 * when 2 was put in
 */
TEST(eq, put_2_get_1)
{
	cevent_tst  e1, e2;
	const cevent_tst  *ep;
	int32_t r1, r2;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	r1 = cqueue_put(&eq, &e1);
	r2 = cqueue_put(&eq, &e2);
	ep = cqueue_get(&eq);

	TEST_ASSERT_EQUAL(0, r1);
	TEST_ASSERT_EQUAL(0, r2);
	TEST_ASSERT(ep == &e1);
}

/**
 * Put 2 events in the queue, get 2, then check addresses
 * and order.
 */
TEST(eq, put_2_get_2)
{
	cevent_tst  e1, e2;
	const cevent_tst  *ep1, *ep2;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	cqueue_put(&eq, &e1);
	cqueue_put(&eq, &e2);
	ep1 = cqueue_get(&eq);
	ep2 = cqueue_get(&eq);

	TEST_ASSERT(ep1 == &e1);
	TEST_ASSERT(ep2 == &e2);
}


/**
 * Check that we get NULL when trying to get from empty queue.
 */
TEST(eq, get_from_empty)
{
	const cevent_tst  *ep1;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	ep1 = cqueue_get(&eq);

	TEST_ASSERT(NULL == ep1);
}

/**
 * Check that cqueue_put returns -1 when there are no more slots.
 */
TEST(eq, put_more_than_capacity)
{
	cevent_tst  e1;
	int32_t r;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	for (uint16_t i=0; i<5; i++)
	{
		r = cqueue_put(&eq, &e1);
	}

	TEST_ASSERT(-1 == r);
}

/**
 * Check that put will roll over, when more events were put/get from
 * the queue.
 */
TEST(eq, test_put_roll)
{
	cevent_tst  e1;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);

	for (uint16_t i=0; i<5; i++)
	{
		cqueue_put(&eq, &e1);
		cqueue_get(&eq);
	}

	TEST_ASSERT(NULL == events[4]);
}

/**
 * Check that get will roll over, when more events were put/get from
 * the queue.
 */
TEST(eq, test_get_roll)
{
	cevent_tst  e1;
	const cevent_tst  *ep1;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);

	for (uint16_t i=0; i<5; i++)
	{
		cqueue_put(&eq, &e1);
		ep1 = cqueue_get(&eq);
	}

	TEST_ASSERT(&e1 == ep1);
}


TEST_GROUP_RUNNER(eq)
{
	RUN_TEST_CASE(eq, put_1_get_1);
	RUN_TEST_CASE(eq, put_2_get_1);
	RUN_TEST_CASE(eq, put_2_get_2);
	RUN_TEST_CASE(eq, get_from_empty);
	RUN_TEST_CASE(eq, put_more_than_capacity);
	RUN_TEST_CASE(eq, test_put_roll);
	RUN_TEST_CASE(eq, test_get_roll);
//	RUN_TEST_CASE(eq, new_1);
//	RUN_TEST_CASE(eq, new_1);
//	RUN_TEST_CASE(eq, new_1);
//	RUN_TEST_CASE(eq, new_1);
//	RUN_TEST_CASE(eq, new_1);
//	RUN_TEST_CASE(eq, new_1);
//	RUN_TEST_CASE(eq, new_1);
//	RUN_TEST_CASE(eq, new_1);
//	RUN_TEST_CASE(eq, new_1);
//	RUN_TEST_CASE(eq, new_1);
}
