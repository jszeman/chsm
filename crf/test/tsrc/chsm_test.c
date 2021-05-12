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


static const cevent_tst event_none = {.sig=C_SIG_NONE, .gc_info=0};

const cevent_tst event1 = {.sig=TEST_SIG1, .gc_info=0};
const cevent_tst event2 = {.sig=TEST_SIG2, .gc_info=0};
const cevent_tst event3 = {.sig=TEST_SIG3, .gc_info=0};

const cevent_tst event_a = {.sig=TEST_SIG_A, .gc_info=0};
const cevent_tst event_b = {.sig=TEST_SIG_B, .gc_info=0};
const cevent_tst event_c = {.sig=TEST_SIG_C, .gc_info=0};
const cevent_tst event_d = {.sig=TEST_SIG_D, .gc_info=0};
const cevent_tst event_e = {.sig=TEST_SIG_E, .gc_info=0};
const cevent_tst event_f = {.sig=TEST_SIG_F, .gc_info=0};
const cevent_tst event_g = {.sig=TEST_SIG_G, .gc_info=0};
const cevent_tst event_h = {.sig=TEST_SIG_H, .gc_info=0};
const cevent_tst event_j = {.sig=TEST_SIG_J, .gc_info=0};
const cevent_tst event_k = {.sig=TEST_SIG_K, .gc_info=0};
const cevent_tst event_l = {.sig=TEST_SIG_L, .gc_info=0};
const cevent_tst event_m = {.sig=TEST_SIG_M, .gc_info=0};
const cevent_tst event_id = {.sig=TEST_SIG_ID, .gc_info=0};

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

/* handle_unknown_event:
 *		Send an event to a state that is not handled anywhere.
 *		The original state shall remain active, that is checked by dispatching
 *		an event that is handled there.
 */

TEST(hsm, handle_unknown_event)
{
	chsm_ctor(&hsm.super, __top__1, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);
	
	clear_log(&hsm);

	chsm_dispatch(&hsm.super, &event2);
	chsm_dispatch(&hsm.super, &event1);

	TEST_ASSERT_EQUAL_STRING("s_sig1_handler ", hsm.log_buff);
}

/* handle_event_in_parent:
 *		Send an event to a state that is only handled in its parent.
 *		The event handler in the parent should be run.
 */

TEST(hsm, handle_event_in_parent)
{
	chsm_ctor(&hsm.super, __top__2, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);
	TEST_ASSERT_EQUAL_STRING("s_entry s_init s1_entry s1_init ", hsm.log_buff);
	
	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event1);
	TEST_ASSERT_EQUAL_STRING("s_sig1_handler ", hsm.log_buff);
}


/* handle_event_in_parent_orig_state:
 *		Send an event to a state that is only handled in its parent.
 *		The original state shall remain active, that is checked by dispatching
 *		an event that is handled there.
 */

TEST(hsm, handle_event_in_parent_orig_state)
{
	chsm_ctor(&hsm.super, __top__2, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);
	chsm_dispatch(&hsm.super, &event1);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event2);
	TEST_ASSERT_EQUAL_STRING("s_sig2_handler ", hsm.log_buff);
}


/* handle_exit_from_child:
 *		Send an event to a state that is only handled in its parent. If
 *		the event processing result in a transition, the exit function
 *		of the child state should be called before executing the transition
 *		function chain.
 */

TEST(hsm, handle_exit_from_child)
{
	chsm_ctor(&hsm.super, __top__3, events, EVENT_QUEUE_SIZE, 0);
	chsm_init(&hsm.super);

	clear_log(&hsm);
	chsm_dispatch(&hsm.super, &event1);

	TEST_ASSERT_EQUAL_STRING("s_sig1_handler s1_exit s2_entry s2_init ", hsm.log_buff);
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
	TEST_ASSERT_EQUAL_STRING("s11_guard k_guard s11_exit s1_exit s1_entry s1_init s11_entry s11_init ", hsm.log_buff);

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
	TEST_ASSERT_EQUAL_STRING("s11_guard k_guard s11_exit s11_entry s11_init ", hsm.log_buff);

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
	TEST_ASSERT_EQUAL_STRING("s11_guard k_guard s1_guard j_guard s11_exit s1_exit s1_entry s11_entry s11_init ", hsm.log_buff);

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
	TEST_ASSERT_EQUAL_STRING("s11_guard k_guard s11_exit s1_exit s2_entry s21_entry s211_entry s211_init ", hsm.log_buff);

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
	TEST_ASSERT_EQUAL_STRING("s11_guard k_guard s11_exit s1_exit s2_entry s2_init s21_entry s21_init s211_entry s211_init ", hsm.log_buff);

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
	TEST_ASSERT_EQUAL_STRING("s11_guard k_guard s1_guard j_guard s11_exit s1_exit ", hsm.log_buff);

	e = hsm.super.event_q_st.get(&(hsm.super.event_q_st));
	TEST_ASSERT_EQUAL(&event_k, e);
	clear_log(&hsm);
	chsm_dispatch(&hsm.super, e);
	TEST_ASSERT_EQUAL_STRING("s3_k_func ", hsm.log_buff);
}

/* defer_1:
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
	TEST_ASSERT_EQUAL_STRING("s11_guard k_guard s1_guard j_guard s11_exit s1_exit ", hsm.log_buff);

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

TEST_GROUP_RUNNER(hsm)
{
	RUN_TEST_CASE(hsm, enter_initial_state);
	RUN_TEST_CASE(hsm, handle_unknown_event);
	RUN_TEST_CASE(hsm, handle_event_in_parent);
	RUN_TEST_CASE(hsm, handle_event_in_parent_orig_state);
	RUN_TEST_CASE(hsm, handle_exit_from_child);
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
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
}
