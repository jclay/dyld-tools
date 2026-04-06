/* Stub for private System/sys/csr.h */
#ifndef _SYSTEM_SYS_CSR_H_STUB
#define _SYSTEM_SYS_CSR_H_STUB

#include <stdint.h>

#define CSR_ALLOW_UNRESTRICTED_FS        (1 << 0)
#define CSR_ALLOW_TASK_FOR_PID           (1 << 1)
#define CSR_ALLOW_UNRESTRICTED_DTRACE    (1 << 5)

static inline int csr_check(__attribute__((unused)) uint32_t mask) { return -1; }

#endif
