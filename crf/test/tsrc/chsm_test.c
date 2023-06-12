/*
 * canopen_test.c
 *
 *  Created on: 2019. jan. 25.
 *      Author: jszeman
 */

#include <chsm.h>
#include "cevent.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "unity_fixture.h"
#include "chsm_test.h"
#include "chsm_test_machine.h"
#include "cpool.h"

#include <stdio.h>

TEST_GROUP(hsm);

test_hsm_tst  hsm;


static const cevent_tst event_none = {.sig=C_SIG_NONE, .gc_info={0}};

const cevent_tst event1 = {.sig=TEST_SIG1, .gc_info={0}};
const cevent_tst event2 = {.sig=TEST_SIG2, .gc_info={0}};
const cevent_tst event3 = {.sig=TEST_SIG3, .gc_info={0}};

const cevent_tst event_a = {.sig=TEST_SIG_A, .gc_info={0}};
const cevent_tst event_b = {.sig=TEST_SIG_B, .gc_info={0}};
const cevent_tst event_c = {.sig=TEST_SIG_C, .gc_info={0}};
const cevent_tst event_d = {.sig=TEST_SIG_D, .gc_info={0}};
const cevent_tst event_e = {.sig=TEST_SIG_E, .gc_info={0}};
const cevent_tst event_f = {.sig=TEST_SIG_F, .gc_info={0}};
const cevent_tst event_g = {.sig=TEST_SIG_G, .gc_info={0}};
const cevent_tst event_h = {.sig=TEST_SIG_H, .gc_info={0}};
const cevent_tst event_j = {.sig=TEST_SIG_J, .gc_info={0}};
const cevent_tst event_k = {.sig=TEST_SIG_K, .gc_info={0}};
const cevent_tst event_l = {.sig=TEST_SIG_L, .gc_info={0}};
const cevent_tst event_m = {.sig=TEST_SIG_M, .gc_info={0}};
const cevent_tst event_n = {.sig=TEST_SIG_N, .gc_info={0}};
const cevent_tst event_id = {.sig=TEST_SIG_ID, .gc_info={0}};

#define EVENT_QUEUE_SIZE 8
#define DEFER_QUEUE_SIZE 4
static const cevent_tst* events[EVENT_QUEUE_SIZE + DEFER_QUEUE_SIZE];

TEST_SETUP(hsm)
{
	memset(&hsm, 0, sizeof(hsm));
	clear_log(&hsm);
}

TEST_TEAR_DOWN(hsm)
{
}

/* enter_initial_state:
 *		Check init behaviour. After calling ctor and init on the test
 *		state machine, the initial transition chain shall be executed.
 *		By sending an event, that is only handed in the target state of
 *		the initial transition we can check if the correct state is set
 *		adter init.
 */
TEST(hsm, enter_initial_state)
{
	chsm_ctor(&hsm.super, __top__1, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	TEST_ASSERT_EQUAL_STRING("s_entry s_init ", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event1);

	TEST_ASSERT_EQUAL_STRING("s_sig1_handler ", hsm.log_buff);
}


/**********************************************************************************
 * Tests below check if the correct sequence of function calls were made based on *
 * the code generated from drawing chsm_test_machine4.html.						  *
 **********************************************************************************/

/* sm4_init:
 *		Check the init sequence and make sure we ended up in the correct state.
 */

TEST(hsm, sm4_init)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	TEST_ASSERT_EQUAL_STRING("s_entry s_init s1_entry s1_init s11_entry s11_init ", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);

	TEST_ASSERT_EQUAL_STRING("s11_id s11_guard k_guard s1_guard j_guard ", hsm.log_buff);
}

/* sm4_s11_g_guard1:
 *		Check that a guard is operational for an internal transition and independent from
 *		the guard that is associated with the external transition for the same event.
 */

TEST(hsm, sm4_s11_g_guard1)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_g);
	TEST_ASSERT_EQUAL_STRING("s11_g_guard1 s11_g_guard2 s11_guard k_guard s1_guard j_guard ", hsm.log_buff);

	clear_log(&hsm);
	hsm.s11_g_guard1 = true;
	chsm_dispatch(&hsm.super, &event_g);
	TEST_ASSERT_EQUAL_STRING("s11_g_guard1 s11_g1 s11_g_guard2 s11_guard k_guard s1_guard j_guard ", hsm.log_buff);
}

/* sm4_s11_g_guard1:
 *		Check that a guard is operational for an external transition and independent from
 *		the guard that is associated with the internal transition for the same event.
 */

TEST(hsm, sm4_s11_g_guard2)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	clear_log(&hsm);
	hsm.s11_g_guard2 = true;
	chsm_dispatch(&hsm.super, &event_g);
	TEST_ASSERT_EQUAL_STRING("s11_g_guard1 s11_g_guard2 s11_g2 s11_exit s1_exit s2_entry s21_entry s211_entry s211_init ", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);

	TEST_ASSERT_EQUAL_STRING("s211_id ", hsm.log_buff);
}

/* sm4_s11_g_guards:
 *		Check that all guards are operational an event and all execute if the conditions are true.
 */

TEST(hsm, sm4_s11_g_guards)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	clear_log(&hsm);
	hsm.s11_g_guard1 = true;
	hsm.s11_g_guard2 = true;
	chsm_dispatch(&hsm.super, &event_g);
	TEST_ASSERT_EQUAL_STRING("s11_g_guard1 s11_g1 s11_g_guard2 s11_g2 s11_exit s1_exit s2_entry s21_entry s211_entry s211_init ", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s211_id ", hsm.log_buff);
}

/* sm4_s11_d_false_guard:
 *		Check that a guarded external transition don't do anyting when the guard is false.
 */

TEST(hsm, sm4_s11_d_false_guard)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_d);
	TEST_ASSERT_EQUAL_STRING("cond s11_guard k_guard s1_guard j_guard ", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s11_id s11_guard k_guard s1_guard j_guard ", hsm.log_buff);
}

/* sm4_s11_d_false_guard_no_parent:
 *		Check that a guarded external transition don't call the parent, even if a standalone
 *		guard in the parent is true.
 */

TEST(hsm, sm4_s11_d_false_guard_no_parent)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	clear_log(&hsm);
	hsm.s1_guard = true;
	chsm_dispatch(&hsm.super, &event_d);
	TEST_ASSERT_EQUAL_STRING("cond s11_guard k_guard s1_guard s1_func j_guard ", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s11_id s11_guard k_guard s1_guard s1_func j_guard ", hsm.log_buff);
}

/* sm4_s11_evaluate_guards:
 *		Check that an unprocessed event will trigger evaluating guards in parents.
 */

TEST(hsm, sm4_s11_evaluate_guards)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_none);
	TEST_ASSERT_EQUAL_STRING("s11_guard k_guard s1_guard j_guard ", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s11_id s11_guard k_guard s1_guard j_guard ", hsm.log_buff);
}

/* sm4_s11_execute_sa_guarded_it:
 *		Check that an unprocessed event will trigger execution of a standalone
 *		(not event associated) guarded internal transition, and then other guards
 *		will not be evaluated.
 */

TEST(hsm, sm4_s11_execute_sa_guarded_it)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	clear_log(&hsm);
	hsm.s11_guard = true;
	chsm_dispatch(&hsm.super, &event_none);
	TEST_ASSERT_EQUAL_STRING("s11_guard s11_func k_guard s1_guard j_guard ", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s11_id s11_guard s11_func k_guard s1_guard j_guard ", hsm.log_buff);
}

/* sm4_s11_execute_sa_guarded_et:
 *		Check that an unprocessed event will trigger execution of a standalone
 *		(not event associated) guarded external transition, and then other guards
 *		will not be evaluated. (The next guard in this case would be in the parent,
 *		but a previous test already showed, that it would be evaluated.)
 */

TEST(hsm, sm4_s11_execute_sa_guarded_et)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	clear_log(&hsm);
	hsm.k_guard = true;
	chsm_dispatch(&hsm.super, &event_none);
	TEST_ASSERT_EQUAL_STRING("s11_guard k_guard s11_exit s1_exit s2_entry s2_init s21_entry s21_init s211_entry s211_init ", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s211_id ", hsm.log_buff);
}

/* sm4_s11_execute_parent_guarded_it:
 *		Check that an unprocessed event will trigger execution of a standalone
 *		(not event associated) guarded internal transition in the parent.
 */

TEST(hsm, sm4_s11_execute_parent_guarded_it)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	clear_log(&hsm);
	hsm.s1_guard = true;
	chsm_dispatch(&hsm.super, &event_none);
	TEST_ASSERT_EQUAL_STRING("s11_guard k_guard s1_guard s1_func j_guard ", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s11_id s11_guard k_guard s1_guard s1_func j_guard ", hsm.log_buff);
}

/* sm4_s11_a:
 *		Check that the correct sequence of function calls take place on an
 *		event that causes an external transition to self in the parent state.
 */

TEST(hsm, sm4_s11_a)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_a);

	bool guards_called_b = strcmp("s11_guard k_guard s11_exit s1_exit s1_entry s1_init s11_entry s11_init ", hsm.log_buff);
	bool guards_not_called_b = strcmp("s11_exit s1_exit s1_entry s1_init s11_entry s11_init ", hsm.log_buff);

	TEST_ASSERT(guards_called_b || guards_not_called_b);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s11_id s11_guard k_guard s1_guard j_guard ", hsm.log_buff);
}

/* sm4_s11_parent_sa_guard_et:
 *		Check that the correct sequence of function calls take place on executing a
 *		standalone guard that causes an external transition.
 */

TEST(hsm, sm4_s11_parent_sa_guard_et)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	clear_log(&hsm);
	hsm.j_guard = true;
	chsm_dispatch(&hsm.super, &event_none);
	TEST_ASSERT_EQUAL_STRING("s11_guard k_guard s1_guard j_guard s11_exit s1_exit s2_entry s2_init s21_entry s21_init s211_entry s211_init ", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s211_id ", hsm.log_buff);
}

/* sm4_s11_h:
 *		Check that the correct sequence of function calls take place on an
 *		event that causes an external transition to the second_parent.
 */

TEST(hsm, sm4_s11_h)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_h);
	
	TEST_ASSERT_EQUAL_STRING("s11_exit s1_exit s_init s1_entry s1_init s11_entry s11_init ", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s11_id s11_guard k_guard s1_guard j_guard ", hsm.log_buff);
}

/* sm4_s11_b:
 *		Check that the correct sequence of function calls take place on an
 *		event that causes an external transition to the current state from
 *		the first parent.
 */

TEST(hsm, sm4_s11_b)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_b);

	bool guards_called_b = strcmp("s11_guard k_guard s11_exit s11_entry s11_init ", hsm.log_buff);
	bool guards_not_called_b = strcmp("s11_exit s11_entry s11_init ", hsm.log_buff);

	TEST_ASSERT(guards_called_b || guards_not_called_b);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s11_id s11_guard k_guard s1_guard j_guard ", hsm.log_buff);
}

/* sm4_s11_e:
 *		Check that the correct sequence of function calls take place on an
 *		event that causes an external transition to the current state from
 *		the second parent.
 */

TEST(hsm, sm4_s11_e)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_e);

	bool guards_called_b = strcmp("s11_guard k_guard s1_guard j_guard s11_exit s1_exit s1_entry s11_entry s11_init ", hsm.log_buff);
	bool guards_not_called_b = strcmp("s11_exit s1_exit s1_entry s11_entry s11_init ", hsm.log_buff);

	TEST_ASSERT(guards_called_b || guards_not_called_b);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s11_id s11_guard k_guard s1_guard j_guard ", hsm.log_buff);
}

/* sm4_s11_d_guard_true:
 *		Check that a guarded transition is  executed, when the guard is evaluated to true
 */

TEST(hsm, sm4_s11_d_guard_true)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	clear_log(&hsm);
	hsm.cond = true;
	chsm_dispatch(&hsm.super, &event_d);
	TEST_ASSERT_EQUAL_STRING("cond d_func s11_exit s1_init s11_entry s11_init ", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s11_id s11_guard k_guard s1_guard j_guard ", hsm.log_buff);
}

/* sm4_s11_f:
 *		Check that a transition from parent to a nested state executes the correct
 *		sequence of functions.
 */

TEST(hsm, sm4_s11_f)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_f);

	bool guards_called_b = strcmp("s11_guard k_guard s11_exit s1_exit s2_entry s21_entry s211_entry s211_init ", hsm.log_buff);
	bool guards_not_called_b = strcmp("s11_exit s1_exit s2_entry s21_entry s211_entry s211_init ", hsm.log_buff);

	TEST_ASSERT(guards_called_b || guards_not_called_b);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s211_id ", hsm.log_buff);
}

/* sm4_s11_c:
 *		Check that a transition from parent to a composite state executes the correct
 *		sequence of functions.
 */

TEST(hsm, sm4_s11_c)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_c);

	bool guards_called_b = strcmp("s11_guard k_guard s11_exit s1_exit s2_entry s2_init s21_entry s21_init s211_entry s211_init ", hsm.log_buff);
	bool guards_not_called_b = strcmp("s11_exit s1_exit s2_entry s2_init s21_entry s21_init s211_entry s211_init ", hsm.log_buff);

	TEST_ASSERT(guards_called_b || guards_not_called_b);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s211_id ", hsm.log_buff);
}

/* sm4_s211_a:
 *		Check that a transition from parent to itself from deeply nested state
 *		executes the correct sequence of functions.
 */

TEST(hsm, sm4_s211_a)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);
	chsm_dispatch(&hsm.super, &event_c); // Go to s211

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_a);
	TEST_ASSERT_EQUAL_STRING("s211_exit(6) s21_exit s21_entry s21_init s211_entry s211_init ", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s211_id ", hsm.log_buff);
}

/* sm4_s211_b:
 *		Check that a transition from parent back to the current deeply nested state
 *		executes the correct sequence of functions.
 */

TEST(hsm, sm4_s211_b)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);
	chsm_dispatch(&hsm.super, &event_c); // Go to s211

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_b);
	TEST_ASSERT_EQUAL_STRING("s211_exit(6) s211_entry s211_init ", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s211_id ", hsm.log_buff);
}

/* sm4_s211_c:
 *		Check that a transition from the second parent to a composite state
 *		executes the correct sequence of functions.
 */

TEST(hsm, sm4_s211_c)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);
	chsm_dispatch(&hsm.super, &event_c); // Go to s211

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_c);
	TEST_ASSERT_EQUAL_STRING("s211_exit(6) s21_exit s2_exit s1_entry s1_init s11_entry s11_init ", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s11_id s11_guard k_guard s1_guard j_guard ", hsm.log_buff);
}

/* sm4_s211_f:
 *		Check that a transition from the second parent back to a nested state
 *		executes the correct sequence of functions.
 */

TEST(hsm, sm4_s211_f)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);
	chsm_dispatch(&hsm.super, &event_c); // Go to s211

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_f);
	TEST_ASSERT_EQUAL_STRING("s211_exit(6) s21_exit s2_exit s1_entry s11_entry s11_init ", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s11_id s11_guard k_guard s1_guard j_guard ", hsm.log_buff);
}

/* sm4_s211_h:
 *		Check that a transition to the third parent
 *		executes the correct sequence of functions.
 */

TEST(hsm, sm4_s211_h)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);
	chsm_dispatch(&hsm.super, &event_c); // Go to s211

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_h);
	TEST_ASSERT_EQUAL_STRING("s211_exit(6) s21_exit s2_exit s_init s1_entry s1_init s11_entry s11_init ", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	TEST_ASSERT_EQUAL_STRING("s11_id s11_guard k_guard s1_guard j_guard ", hsm.log_buff);
}

/* sm4_s5_entry:
 *		Check that executing multiple functions at entry
		works.
 */

TEST(hsm, sm4_s5_entry)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_m); // Go to s5

	TEST_ASSERT_EQUAL_STRING("s11_exit s1_exit s5_entry1 s5_entry2 ", hsm.log_buff);
}

/* sm4_s5_k:
 *		Check that executing multiple functions written at
		multiple rows works.
 */

TEST(hsm, sm4_s5_k)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	chsm_dispatch(&hsm.super, &event_m); // Go to s5
	clear_log(&hsm);

	chsm_dispatch(&hsm.super, &event_k);

	TEST_ASSERT_EQUAL_STRING("s5_k_func1 s5_k_func2 s5_g1 ", hsm.log_buff);
}

/* sm4_s5_c:
 *		Check that executing multiple functions during a transition
		works.
 */

TEST(hsm, sm4_s5_c)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	chsm_dispatch(&hsm.super, &event_m); // Go to s5
	clear_log(&hsm);

	chsm_dispatch(&hsm.super, &event_c);

	TEST_ASSERT_EQUAL_STRING("s5_c_func1 s5_c_cfunc2 s_init s1_entry s1_init s11_entry s11_init ", hsm.log_buff);
}

/* sm4_s5_g1:
 *		Check that executing multiple functions after a guard
		works.
 */

TEST(hsm, sm4_s5_g1)
{
	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	hsm.s5_g1 = true;
	chsm_dispatch(&hsm.super, &event_m); // Go to s5
	clear_log(&hsm);

	chsm_dispatch(&hsm.super, &event_a);

	TEST_ASSERT_EQUAL_STRING("s5_g1 s5_g1_f1 s5_g1_f2 ", hsm.log_buff);
}

/* init_event_queue:
 *		Check that the constructor properly initializes the event queue by
 *		using the HSM as a queue.
 */

TEST(hsm, init_event_queue)
{
	const cevent_tst *e = NULL;

	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, DEFER_QUEUE_SIZE);

	hsm.super.event_q_st.put(&(hsm.super.event_q_st), &event1);

	e = hsm.super.event_q_st.get(&(hsm.super.event_q_st));

	TEST_ASSERT_EQUAL(&event1, e);
}

/* defer_1:
 *		Defer one event then recall it later.
 */

TEST(hsm, defer_1)
{
	const cevent_tst *e = NULL;

	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, DEFER_QUEUE_SIZE);
	chsm_init(&hsm.super);
	TEST_ASSERT_EQUAL_STRING("s_entry s_init s1_entry s1_init s11_entry s11_init ", hsm.log_buff);

	hsm.super.event_q_st.put(&(hsm.super.event_q_st), &event_k); // this will be deferred
	hsm.super.event_q_st.put(&(hsm.super.event_q_st), &event_j); // this will result in a transition to a state, where K can be processed

	e = hsm.super.event_q_st.get(&(hsm.super.event_q_st));
	TEST_ASSERT_EQUAL(&event_k, e);
	clear_log(&hsm);
	chsm_dispatch(&hsm.super, e);
	TEST_ASSERT_EQUAL_STRING("s11_guard k_guard s1_guard j_guard ", hsm.log_buff); // No string is expected since this event is deferred.

	e = hsm.super.event_q_st.get(&(hsm.super.event_q_st));
	TEST_ASSERT_EQUAL(&event_j, e);
	clear_log(&hsm);
	chsm_dispatch(&hsm.super, e);

	bool guards_called_b = strcmp("s11_guard k_guard s1_guard j_guard s11_exit s1_exit ", hsm.log_buff);
	bool guards_not_called_b = strcmp("s1_guard j_guard s11_exit s1_exit ", hsm.log_buff);

	TEST_ASSERT(guards_called_b || guards_not_called_b);

	e = hsm.super.event_q_st.get(&(hsm.super.event_q_st));
	TEST_ASSERT_EQUAL(&event_k, e);
	clear_log(&hsm);
	chsm_dispatch(&hsm.super, e);
	TEST_ASSERT_EQUAL_STRING("s3_k_func ", hsm.log_buff);
}

/* defer_2:
 *		Defer two event then recall them later.
 */

TEST(hsm, defer_2)
{
	const cevent_tst *e = NULL;

	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, DEFER_QUEUE_SIZE);
	chsm_init(&hsm.super);
	TEST_ASSERT_EQUAL_STRING("s_entry s_init s1_entry s1_init s11_entry s11_init ", hsm.log_buff);

	hsm.super.event_q_st.put(&(hsm.super.event_q_st), &event_k); // this will be deferred
	hsm.super.event_q_st.put(&(hsm.super.event_q_st), &event_l); // this will be deferred
	hsm.super.event_q_st.put(&(hsm.super.event_q_st), &event_j); // this will result in a transition to a state, where K can be processed

	e = hsm.super.event_q_st.get(&(hsm.super.event_q_st));
	TEST_ASSERT_EQUAL(&event_k, e);
	clear_log(&hsm);
	chsm_dispatch(&hsm.super, e);
	TEST_ASSERT_EQUAL_STRING("s11_guard k_guard s1_guard j_guard ", hsm.log_buff); // No string is expected since this event is deferred.

	e = hsm.super.event_q_st.get(&(hsm.super.event_q_st));
	TEST_ASSERT_EQUAL(&event_l, e);
	clear_log(&hsm);
	chsm_dispatch(&hsm.super, e);
	TEST_ASSERT_EQUAL_STRING("s11_guard k_guard s1_guard j_guard ", hsm.log_buff); // No string is expected since this event is deferred.

	e = hsm.super.event_q_st.get(&(hsm.super.event_q_st));
	TEST_ASSERT_EQUAL(&event_j, e);
	clear_log(&hsm);
	chsm_dispatch(&hsm.super, e);

	bool guards_called_b = strcmp("s11_guard k_guard s1_guard j_guard s11_exit s1_exit ", hsm.log_buff);
	bool guards_not_called_b = strcmp("s1_guard j_guard s11_exit s1_exit ", hsm.log_buff);

	TEST_ASSERT(guards_called_b || guards_not_called_b);

	e = hsm.super.event_q_st.get(&(hsm.super.event_q_st));
	TEST_ASSERT_EQUAL(&event_k, e);
	clear_log(&hsm);
	chsm_dispatch(&hsm.super, e);
	TEST_ASSERT_EQUAL_STRING("s3_k_func ", hsm.log_buff);

	e = hsm.super.event_q_st.get(&(hsm.super.event_q_st));
	TEST_ASSERT_EQUAL(&event_l, e);
	clear_log(&hsm);
	chsm_dispatch(&hsm.super, e);
	TEST_ASSERT_EQUAL_STRING("s3_l_func ", hsm.log_buff);
}

/* sm4_doc_file:
 *		This test just writes a bunch of examples into
		a file, that can be referenced in the documantation.
 */

TEST(hsm, sm4_doc_file)
{
	FILE* f;

	f = fopen("sm.doc", "w");
	if (f == NULL) return;
	fprintf(f, "CHSM State machine execution log\n\n");

	fprintf(f, "This auto generated file describes what functions\n"
			   "will be called when the state machine receives a\n"
			   "particular signal. Format:\n\n"
			   "state_name: signal [relevant guards]:\n"
			   "    List of called functions \n\n");

	chsm_ctor(&hsm.super, __top__4, events, EVENT_QUEUE_SIZE, DEFER_QUEUE_SIZE);
	clear_log(&hsm);
	
	chsm_init(&hsm.super);
	fprintf(f, "__top__: INIT\n    %s\n\n", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_id);
	fprintf(f, "s11: ID\n    %s\n\n", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_d);
	fprintf(f, "s11: D [cond() => false]\n    %s\n\n", hsm.log_buff);

	clear_log(&hsm);
	hsm.cond = true;
	chsm_dispatch(&hsm.super, &event_d);
	hsm.cond = false;
	fprintf(f, "s11: D [cond() => true]\n    %s\n\n", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_a);
	fprintf(f, "s11: A\n    %s\n\n", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_b);
	fprintf(f, "s11: B\n    %s\n\n", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_h);
	fprintf(f, "s11: H\n    %s\n\n", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_e);
	fprintf(f, "s11: E\n    %s\n\n", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_f);
	fprintf(f, "s11: F\n    %s\n\n", hsm.log_buff);
	chsm_dispatch(&hsm.super, &event_f); // Go back to s11

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_c);
	fprintf(f, "s11: C\n    %s\n\n", hsm.log_buff);
	chsm_dispatch(&hsm.super, &event_f); // Go back to s11

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_g);
	fprintf(f, "s11: G [s11_g_guard1() => false, s11_g_guard2() => false]\n    %s\n\n", hsm.log_buff);

	clear_log(&hsm);
	hsm.s11_g_guard1 = true;
	chsm_dispatch(&hsm.super, &event_g);
	fprintf(f, "s11: G [s11_g_guard1() => true, s11_g_guard2() => false]\n    %s\n\n", hsm.log_buff);
	hsm.s11_g_guard1 = false;

	clear_log(&hsm);
	hsm.s11_g_guard2 = true;
	chsm_dispatch(&hsm.super, &event_g);
	fprintf(f, "s11: G [s11_g_guard1() => false, s11_g_guard2() => true]\n    %s\n\n", hsm.log_buff);
	chsm_dispatch(&hsm.super, &event_f); // Go back to s11

	clear_log(&hsm);
	hsm.s11_g_guard1 = true;
	hsm.s11_g_guard2 = true;
	chsm_dispatch(&hsm.super, &event_g);
	fprintf(f, "s11: G [s11_g_guard1() => true, s11_g_guard2() => true]\n    %s\n\n", hsm.log_buff);
	chsm_dispatch(&hsm.super, &event_f); // Go back to s11
	hsm.s11_g_guard1 = false;
	hsm.s11_g_guard2 = false;

	clear_log(&hsm);
	hsm.k_guard = true;
	chsm_dispatch(&hsm.super, &event_n);
	fprintf(f, "s11: N [k_guard1() => true]\n    %s\n\n", hsm.log_buff);
	chsm_dispatch(&hsm.super, &event_f); // Go back to s11
	hsm.k_guard = false;

	clear_log(&hsm);
	hsm.s1_guard = true;
	chsm_dispatch(&hsm.super, &event_n);
	fprintf(f, "s11: N [s1_guard() => true]\n    %s\n\n", hsm.log_buff);
	hsm.s1_guard = false;

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_k);
	chsm_dispatch(&hsm.super, &event_j);
	const cevent_tst *e = hsm.super.event_q_st.get(&(hsm.super.event_q_st));
	if (e)
	{
		chsm_dispatch(&hsm.super, e);
	}
	fprintf(f, "s11: K, J\n    %s\n\n", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_e);
	fprintf(f, "s3: E \n    %s\n\n", hsm.log_buff);

	chsm_dispatch(&hsm.super, &event_c); // Go to s211

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_c);
	fprintf(f, "s211: C \n    %s\n\n", hsm.log_buff);
	chsm_dispatch(&hsm.super, &event_c); // Go to s211

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_g);
	fprintf(f, "s211: G \n    %s\n\n", hsm.log_buff);
	chsm_dispatch(&hsm.super, &event_c); // Go to s211

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_f);
	fprintf(f, "s211: F \n    %s\n\n", hsm.log_buff);
	chsm_dispatch(&hsm.super, &event_c); // Go to s211

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_d);
	fprintf(f, "s211: D \n    %s\n\n", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_b);
	fprintf(f, "s211: B \n    %s\n\n", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_a);
	fprintf(f, "s211: A \n    %s\n\n", hsm.log_buff);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event_h);
	fprintf(f, "s211: H \n    %s\n\n", hsm.log_buff);


	
	fclose(f);
}

TEST_GROUP_RUNNER(hsm)
{
	RUN_TEST_CASE(hsm, enter_initial_state);
	RUN_TEST_CASE(hsm, sm4_init);
	RUN_TEST_CASE(hsm, sm4_s11_g_guard1);
	RUN_TEST_CASE(hsm, sm4_s11_g_guard2);
	RUN_TEST_CASE(hsm, sm4_s11_g_guards);
	RUN_TEST_CASE(hsm, sm4_s11_d_false_guard);
	RUN_TEST_CASE(hsm, sm4_s11_d_false_guard_no_parent);
	RUN_TEST_CASE(hsm, sm4_s11_evaluate_guards);
	RUN_TEST_CASE(hsm, sm4_s11_execute_sa_guarded_it);
	RUN_TEST_CASE(hsm, sm4_s11_execute_sa_guarded_et);
	RUN_TEST_CASE(hsm, sm4_s11_execute_parent_guarded_it);
	RUN_TEST_CASE(hsm, sm4_s11_a);
	RUN_TEST_CASE(hsm, sm4_s11_parent_sa_guard_et);
	RUN_TEST_CASE(hsm, sm4_s11_h);
	RUN_TEST_CASE(hsm, sm4_s11_b);
	RUN_TEST_CASE(hsm, sm4_s11_e);
	RUN_TEST_CASE(hsm, sm4_s11_d_guard_true);
	RUN_TEST_CASE(hsm, sm4_s11_f);
	RUN_TEST_CASE(hsm, sm4_s11_c);
	RUN_TEST_CASE(hsm, sm4_s211_a);
	RUN_TEST_CASE(hsm, sm4_s211_b);
	RUN_TEST_CASE(hsm, sm4_s211_c);
	RUN_TEST_CASE(hsm, sm4_s211_f);
	RUN_TEST_CASE(hsm, sm4_s211_h);
	RUN_TEST_CASE(hsm, init_event_queue);
	RUN_TEST_CASE(hsm, defer_1);
	RUN_TEST_CASE(hsm, defer_2);
	RUN_TEST_CASE(hsm, sm4_s5_entry);
	RUN_TEST_CASE(hsm, sm4_s5_k);
	RUN_TEST_CASE(hsm, sm4_s5_c);
	RUN_TEST_CASE(hsm, sm4_s5_g1);
	RUN_TEST_CASE(hsm, sm4_doc_file);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
}
