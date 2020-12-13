#ifndef ATOMIC_OPS_H_
#define ATOMIC_OPS_H_

#include <stdint.h>
#include <stdbool.h>

#if (__STDC_VERSION__ >= 201112L)
    #ifndef __STDC_NO_ATOMICS__
        #include <stdatomic.h>
        #define atomic_compare_exchange_u16(obj, expected, desired) atomic_compare_exchange_weak(obj, expected, desired)
        #define atomic_fetch_add_u16(obj, value) atomic_fetch_add(obj, value)

        typedef atomic_uint_least16_t atomic_uint16_t;

        #define BUILTIN_ATOMICS
    #endif
#endif

#ifndef BUILTIN_ATOMICS
    typedef volatile uint16_t atomic_uint16_t;
    /* atomic_compare_exchange_u16:
     * Atomically compares the contents of memory pointed to by obj with the
     * contents of memory pointed to by expected, and if those are bitwise equal,
     * replaces the former with desired (performs read-modify-write operation).
     * Otherwise, loads the actual contents of memory pointed to by obj into
     * *expected (performs load operation).
     */
    bool atomic_compare_exchange_u16(atomic_uint16_t *obj, uint16_t *expected, uint16_t desired);

    /* atomic_fetch_add_u16:
     * Atomically replaces the value pointed by obj with the result of addition of
     * value to the old value of obj, and returns the value obj held previously.
     * The operation is read-modify-write operation.
     */
    uint16_t atomic_fetch_add_u16(atomic_uint16_t *obj, uint16_t value);
#endif

#endif
