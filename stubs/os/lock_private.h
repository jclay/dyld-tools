/* Stub for private os/lock_private.h */
#ifndef _OS_LOCK_PRIVATE_H_STUB
#define _OS_LOCK_PRIVATE_H_STUB

#include <os/lock.h>
#include <pthread.h>

/* os_unfair_lock_lock_with_options — private version with options.
 * Just forward to the public os_unfair_lock_lock. */
typedef uint32_t os_unfair_lock_options_t;
#define OS_UNFAIR_LOCK_NONE 0
#define OS_UNFAIR_LOCK_DATA_SYNCHRONIZATION 0x00010000

static inline void os_unfair_lock_lock_with_options(os_unfair_lock_t lock,
                                                     os_unfair_lock_options_t options __attribute__((unused))) {
    os_unfair_lock_lock(lock);
}

/* os_unfair_recursive_lock — private recursive unfair lock */
typedef struct os_unfair_recursive_lock_s {
    os_unfair_lock ourl_lock;
    uint32_t       ourl_count;
} os_unfair_recursive_lock, *os_unfair_recursive_lock_t;

#define OS_UNFAIR_RECURSIVE_LOCK_INIT {OS_UNFAIR_LOCK_INIT, 0}

static inline void os_unfair_recursive_lock_lock(os_unfair_recursive_lock *lock) {
    os_unfair_lock_lock(&lock->ourl_lock);
    lock->ourl_count++;
}

static inline void os_unfair_recursive_lock_unlock(os_unfair_recursive_lock *lock) {
    lock->ourl_count--;
    os_unfair_lock_unlock(&lock->ourl_lock);
}

#endif
