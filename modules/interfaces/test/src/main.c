#include <stdio.h>
#include "unity_fixture.h"
#include "unity.h"

void disableInterrupts(void)
{

}

void enableInterrupts(void)
{

}

void run_intf_tests(void)
{
	RUN_TEST_GROUP(i2c_drv_mock);
	RUN_TEST_GROUP(spi_drv_mock);
}

int main(int argc, const char * argv[])
{
	return UnityMain(argc, argv, run_intf_tests);
}