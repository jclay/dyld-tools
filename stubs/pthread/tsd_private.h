/* Stub for private pthread/tsd_private.h */
#ifndef _PTHREAD_TSD_PRIVATE_H_STUB
#define _PTHREAD_TSD_PRIVATE_H_STUB

#include <pthread.h>

/* Thread-specific data slots used by dyld */
#define _PTHREAD_TSD_SLOT_DYLD_1 6
#define _PTHREAD_TSD_SLOT_DYLD_2 7

static inline void* _pthread_getspecific_direct(unsigned long slot) {
    /* Private TSD access — fallback to pthread_getspecific is not exact,
     * but sufficient for tool builds where this code path is inactive */
    (void)slot;
    return NULL;
}

static inline void _pthread_setspecific_direct(unsigned long slot, void *val) {
    (void)slot;
    (void)val;
}

#endif
