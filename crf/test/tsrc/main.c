/*
 * main.c
 *
 *  Created on: 2019. jan. 25.
 *      Author: jszeman
 */

#include <stdio.h>
#include "unity_fixture.h"
#include "unity.h"

void disableInterrupts(void)
{

}

void enableInterrupts(void)
{

}

void run_hsm_tests(void)
{
	RUN_TEST_GROUP(ep);
	RUN_TEST_GROUP(eq);
	RUN_TEST_GROUP(hsm);
}

int main(int argc, char * argv[])
{
	return UnityMain(argc, argv, run_hsm_tests);
}
