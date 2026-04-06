/* Stub for private os/atomic_private.h */
#ifndef _OS_ATOMIC_PRIVATE_H_STUB
#define _OS_ATOMIC_PRIVATE_H_STUB

/* DO NOT include <stdatomic.h> here — its C macros conflict with C++ std::atomic.
 * Use compiler builtins directly instead. */

#define os_atomic_load(p, order) __atomic_load_n(p, __ATOMIC_SEQ_CST)
#define os_atomic_store(p, v, order) __atomic_store_n(p, v, __ATOMIC_SEQ_CST)
#define os_atomic_cmpxchg(p, e, v, order) __sync_bool_compare_and_swap(p, e, v)
#define os_atomic_add(p, v, order) __sync_fetch_and_add(p, v)
#define os_atomic_or(p, v, order) __sync_fetch_and_or(p, v)
#define os_atomic_and(p, v, order) __sync_fetch_and_and(p, v)
#define os_atomic_xchg(p, v, order) __sync_lock_test_and_set(p, v)
#define os_atomic_inc(p, order) __sync_fetch_and_add(p, 1)
#define os_atomic_dec(p, order) __sync_fetch_and_sub(p, 1)
#define os_atomic_rmw_loop(p, ov, nv, order, expr) ({ \
    __typeof__(*(p)) ov = os_atomic_load(p, order); \
    bool _r; \
    do { __typeof__(*(p)) nv; expr; \
        _r = os_atomic_cmpxchg(p, ov, nv, order); \
        if (!_r) ov = os_atomic_load(p, order); \
    } while (!_r); _r; })
#define os_atomic_rmw_loop_give_up(expr) ({ expr; break; })

#endif
