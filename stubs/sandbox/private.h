/* Stub for private sandbox/private.h */
#ifndef _SANDBOX_PRIVATE_H_STUB
#define _SANDBOX_PRIVATE_H_STUB

#include <sandbox.h>

/* sandbox_filter_type — used for sandbox_check_by_audit_token */
typedef enum {
    SANDBOX_FILTER_NONE = 0,
    SANDBOX_FILTER_PATH = 1,
    SANDBOX_FILTER_GLOBAL_NAME = 2,
    SANDBOX_FILTER_LOCAL_NAME = 3,
    SANDBOX_FILTER_APPLEEVENT_DESTINATION = 4,
    SANDBOX_FILTER_RIGHT_NAME = 5,
    SANDBOX_FILTER_PREFERENCE_DOMAIN = 6,
    SANDBOX_FILTER_KEXT_BUNDLE_ID = 7,
    SANDBOX_FILTER_INFO_TYPE = 8,
    SANDBOX_FILTER_NOTIFICATION = 9,
} sandbox_filter_type;

#define SANDBOX_CHECK_NO_REPORT 0x0001

static inline int sandbox_check(pid_t pid __attribute__((unused)),
                                 const char *operation __attribute__((unused)),
                                 sandbox_filter_type type __attribute__((unused)), ...) {
    return 0; /* allow everything in non-sandbox builds */
}

#endif
