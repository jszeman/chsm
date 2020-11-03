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
	r = eq.put(&eq, &e);
	ep = eq.get(&eq);

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
	r1 = eq.put(&eq, &e1);
	r2 = eq.put(&eq, &e2);
	ep = eq.get(&eq);

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
	eq.put(&eq, &e1);
	eq.put(&eq, &e2);
	ep1 = eq.get(&eq);
	ep2 = eq.get(&eq);

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
	ep1 = eq.get(&eq);

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
		r = eq.put(&eq, &e1);
	}

	TEST_ASSERT(-1 == r);
}

/**
 * Check that we can get an element even if the queue is full
 */
TEST(eq, get_all)
{
	cevent_tst  e[4];
	const cevent_tst  *ep[4];
	int32_t r;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	for (uint16_t i=0; i<4; i++)
	{
		r = eq.put(&eq, &e[i]);
	}

	for (uint16_t i=0; i<4; i++)
	{
		ep[i] = eq.get(&eq);

		TEST_ASSERT(ep[i] == &e[i]);
	}

	
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
		eq.put(&eq, &e1);
		eq.get(&eq);
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
		eq.put(&eq, &e1);
		ep1 = eq.get(&eq);
	}

	TEST_ASSERT(&e1 == ep1);
}

/**
 * Check that put_left will allow getting the pointer back.
 */
TEST(eq, put_left_1_get_1)
{
	cevent_tst  e;
	const cevent_tst  *ep;
	int32_t r;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	r = eq.put_left(&eq, &e);
	ep = eq.get(&eq);

	TEST_ASSERT_EQUAL(0, r);
	TEST_ASSERT(ep == &e);
}

/**
 * Check that put_left will result in a stack like operation,
 * when the items are read back in reverse order.
 */
TEST(eq, put_left_2_get_2)
{
	cevent_tst  e1, e2;
	const cevent_tst  *ep1, *ep2;
	int32_t r1, r2;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	r1 = eq.put_left(&eq, &e1);
	r2 = eq.put_left(&eq, &e2);
	ep1 = eq.get(&eq);
	ep2 = eq.get(&eq);

	TEST_ASSERT_EQUAL(0, r1);
	TEST_ASSERT_EQUAL(0, r2);
	TEST_ASSERT(ep1 == &e2);
	TEST_ASSERT(ep2 == &e1);
}

/**
 * Check that put_left is able to insert an element in
 * front of the previously put in items.
 */
TEST(eq, put_left_after_put)
{
	cevent_tst  e1, e2, e3;
	const cevent_tst  *ep1;
	int32_t r1, r2, r3;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	r1 = eq.put(&eq, &e1);
	r2 = eq.put(&eq, &e2);
	r3 = eq.put_left(&eq, &e3);
	ep1 = eq.get(&eq);

	TEST_ASSERT_EQUAL(0, r1);
	TEST_ASSERT_EQUAL(0, r2);
	TEST_ASSERT_EQUAL(0, r3);
	TEST_ASSERT(ep1 == &e3);
}

/**
 * Check that put_left returns -1 when there are no more slots.
 */
TEST(eq, put_left_more_than_capacity)
{
	cevent_tst  e1;
	int32_t r;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	for (uint16_t i=0; i<5; i++)
	{
		r = eq.put_left(&eq, &e1);
		printf("%u - %u = %u\n", eq.head, eq.tail, (uint16_t)(eq.head - eq.tail));
	}

	TEST_ASSERT(-1 == r);
}

/**
 * Test that we can get back the last event from the queue,
 * when 2 was put in
 */
TEST(eq, put_2_get_right_1)
{
	cevent_tst  e1, e2;
	const cevent_tst  *ep;
	int32_t r1, r2;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	r1 = eq.put(&eq, &e1);
	r2 = eq.put(&eq, &e2);
	ep = eq.get_right(&eq);

	TEST_ASSERT_EQUAL(0, r1);
	TEST_ASSERT_EQUAL(0, r2);
	TEST_ASSERT(ep == &e2);
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
	RUN_TEST_CASE(eq, put_left_1_get_1);
	RUN_TEST_CASE(eq, put_left_2_get_2);
	RUN_TEST_CASE(eq, put_left_after_put);
	RUN_TEST_CASE(eq, put_left_more_than_capacity);
	RUN_TEST_CASE(eq, get_all);
	RUN_TEST_CASE(eq, put_2_get_right_1);
//	RUN_TEST_CASE(eq, new_1);
//	RUN_TEST_CASE(eq, new_1);
//	RUN_TEST_CASE(eq, new_1);
//	RUN_TEST_CASE(eq, new_1);
}
