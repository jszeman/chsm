#include <stdio.h>
#include "unity_fixture.h"
#include "unity.h"

void disableInterrupts(void)
{

}

void enableInterrupts(void)
{

}

void run_ltc2471_tests(void)
{
	RUN_TEST_GROUP(ltc2471);
}

int main(int argc, const char * argv[])
{
	return UnityMain(argc, argv, run_ltc2471_tests);
}