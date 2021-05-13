#include <stdio.h>
#include "unity_fixture.h"
#include "unity.h"

void disableInterrupts(void)
{

}

void enableInterrupts(void)
{

}

void run_dac8565_tests(void)
{
	RUN_TEST_GROUP(dac8565);
}

int main(int argc, const char * argv[])
{
	return UnityMain(argc, argv, run_dac8565_tests);
}