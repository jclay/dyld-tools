/* Stub for private sys/kdebug_private.h */
#ifndef _SYS_KDEBUG_PRIVATE_H_STUB
#define _SYS_KDEBUG_PRIVATE_H_STUB

#include <sys/kdebug.h>
#include <stdint.h>

#ifndef DBG_FUNC_START
#define DBG_FUNC_START 1
#endif
#ifndef DBG_FUNC_END
#define DBG_FUNC_END   2
#endif
#ifndef DBG_FUNC_ALL
#define DBG_FUNC_ALL   (DBG_FUNC_START | DBG_FUNC_END)
#endif

#ifndef DBG_DYLD
#define DBG_DYLD       31
#endif

#ifndef DBG_DYLD_UUID
#define DBG_DYLD_UUID  5
#endif

#ifndef KDBG_CODE
#define KDBG_CODE(cls, sub, code) (((cls) << 24) | ((sub) << 16) | ((code) << 2))
#endif

static inline uint64_t kdebug_trace_string(__attribute__((unused)) uint32_t code,
                                            __attribute__((unused)) uint64_t str_id,
                                            __attribute__((unused)) const char *str) {
    return 0;
}

static inline int kdebug_trace(__attribute__((unused)) uint32_t code, ...) {
    return 0;
}

static inline bool kdebug_is_enabled(__attribute__((unused)) uint32_t code) {
    return false;
}

#endif
