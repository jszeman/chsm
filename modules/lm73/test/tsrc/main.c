#include <stdio.h>
#include "unity_fixture.h"
#include "unity.h"

void disableInterrupts(void)
{

}

void enableInterrupts(void)
{

}

void run_lm73_tests(void)
{
	RUN_TEST_GROUP(lm73);
}

int main(int argc, const char * argv[])
{
	return UnityMain(argc, argv, run_lm73_tests);
}