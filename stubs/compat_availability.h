/* Compatibility defines for availability macros not in the public SDK.
 * Force-included before all source files via -include.
 */
#ifndef _COMPAT_AVAILABILITY_H
#define _COMPAT_AVAILABILITY_H

#ifndef DYLD_EXCLAVEKIT_UNAVAILABLE
#define DYLD_EXCLAVEKIT_UNAVAILABLE
#endif

/* __OSX_DEPRECATED may not exist on older SDK versions */
#ifndef __OSX_DEPRECATED
#define __OSX_DEPRECATED(introduced, deprecated, message)
#endif

/* The public macOS SDK doesn't know about bridgeos/exclavekit platforms.
 * Availability macros like __API_AVAILABLE(bridgeos(5.0)) expand to
 * __attribute__((availability(__API_AVAILABLE_PLATFORM_bridgeos, ...)))
 * which fails when the platform macro isn't defined. Define them as empty. */
#ifndef __API_AVAILABLE_PLATFORM_bridgeos
#define __API_AVAILABLE_PLATFORM_bridgeos(x)
#endif
#ifndef __API_UNAVAILABLE_PLATFORM_bridgeos
#define __API_UNAVAILABLE_PLATFORM_bridgeos
#endif
#ifndef __SPI_AVAILABLE_PLATFORM_bridgeos
#define __SPI_AVAILABLE_PLATFORM_bridgeos(x)
#endif

/* TARGET_OS_EXCLAVEKIT may not be defined in older SDKs */
#ifndef TARGET_OS_EXCLAVEKIT
#define TARGET_OS_EXCLAVEKIT 0
#endif

/* TARGET_OS_DRIVERKIT may not be defined */
#ifndef TARGET_OS_DRIVERKIT
#define TARGET_OS_DRIVERKIT 0
#endif

/* abort_report_np — private libc function, exported from libsystem_c.dylib. */
#ifdef __cplusplus
extern "C" {
#endif
__attribute__((noreturn, format(printf, 1, 2)))
void abort_report_np(const char *fmt, ...);
#ifdef __cplusplus
}
#endif

/* PLATFORM_IOSMAC is the old name for PLATFORM_MACCATALYST */
#ifndef PLATFORM_IOSMAC
#define PLATFORM_IOSMAC PLATFORM_MACCATALYST
#endif

/* openbyid_np — private syscall for opening files by ID */
#ifdef __cplusplus
extern "C" {
#endif
static inline int openbyid_np(void *fsid __attribute__((unused)),
                               void *objid __attribute__((unused)),
                               int oflags __attribute__((unused))) {
    return -1; /* stub — always fail */
}
/* pthread_key_init_np — private pthread API */
static inline int pthread_key_init_np(int slot __attribute__((unused)),
                                       void (*destructor)(void*) __attribute__((unused))) {
    return 0;
}
#ifdef __cplusplus
}
#endif

/* Common system headers needed by various dyld sources */
#include <assert.h>
#include <sys/attr.h>
#include <sys/mount.h>

/* task_read_for_pid is private on macOS — fall back to task_for_pid */
#ifndef task_read_for_pid
#define task_read_for_pid task_for_pid
#endif

/* abort_with_payload — private syscall, exported from libsystem_kernel.dylib.
 * Declaration provided by xnu bsd/sys/reason.h (fetched at configure time). */

#endif
