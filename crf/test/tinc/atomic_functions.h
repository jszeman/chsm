#include "atomic_functions.h"

bool atomic_compare_exchange_u16(atomic_uint16_t *obj, uint16_t *expected, uint16_t desired);
uint16_t atomic_fetch_add_u16(atomic_uint16_t *obj, uint16_t value);
