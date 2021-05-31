#include "stdint.h"
#include "stdbool.h"

cpu_c28x_enable_interrupts(){}
cpu_c28x_disable_interrupts(){}

typedef volatile uint16_t atomic_uint16_t;
typedef volatile int16_t atomic_int16_t;
  /* atomic_compare_exchange_u16:
   * Atomically compares the contents of memory pointed to by obj with the
   * contents of memory pointed to by expected, and if those are bitwise equal,
   * replaces the former with desired (performs read-modify-write operation).
   * Otherwise, loads the actual contents of memory pointed to by obj into
   * *expected (performs load operation).
   */
bool atomic_compare_exchange_u16(atomic_uint16_t *obj, uint16_t *expected, uint16_t desired)
{
  cpu_c28x_disable_interrupts();

  if(*obj == *expected)
  {
      *obj = desired;
      cpu_c28x_enable_interrupts();
      return true;
  }
  else
  {
      *expected = *obj;
      cpu_c28x_enable_interrupts();
      return false;
  }

}

  /* atomic_fetch_add_u16:
   * Atomically replaces the value pointed by obj with the result of addition of
   * value to the old value of obj, and returns the value obj held previously.
   * The operation is read-modify-write operation.
   */
uint16_t atomic_fetch_add_u16(atomic_uint16_t *obj, uint16_t value)
{
    atomic_uint16_t tmp = *obj;

    cpu_c28x_disable_interrupts();

    *obj += value;

    cpu_c28x_enable_interrupts();

    return tmp;
}