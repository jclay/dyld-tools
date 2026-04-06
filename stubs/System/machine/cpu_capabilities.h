/* Stub for private System/machine/cpu_capabilities.h */
#ifndef _SYSTEM_MACHINE_CPU_CAPABILITIES_H_STUB
#define _SYSTEM_MACHINE_CPU_CAPABILITIES_H_STUB

#include <stdint.h>

/* Comm page addresses for CPU capabilities */
#if __x86_64__
#define _COMM_PAGE_CPU_CAPABILITIES64    ((uintptr_t)0x00007fffffe0003C)
#define _COMM_PAGE_CPU_CAPABILITIES      ((uintptr_t)0x00007fffffe00020)
#define kHasSSE4_2                       0x00004000
#elif __arm64__
#define _COMM_PAGE_CPU_CAPABILITIES      ((uintptr_t)0x0000000FFFFFC020)
#define kHasARMv8Crc32                   0x0001
#endif

#endif
