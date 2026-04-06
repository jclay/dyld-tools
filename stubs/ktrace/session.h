/* Stub for private ktrace/session.h
 * dyld_usage.cpp requires the full ktrace session API.
 * This stub provides type declarations but the tool won't work
 * at runtime without the real libktrace.
 */
#ifndef _KTRACE_SESSION_H_STUB
#define _KTRACE_SESSION_H_STUB

#include <stdint.h>
#include <stdbool.h>
#include <dispatch/dispatch.h>
#include <sys/kdebug.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ktrace_session_s* ktrace_session_t;

/* Event structure passed to callbacks */
typedef struct {
    uint32_t debugid;
    uint64_t timestamp;
    uint64_t arg1;
    uint64_t arg2;
    uint64_t arg3;
    uint64_t arg4;
    int      threadid;
    int      cpuid;
    const char *command;
    uint64_t walltime;
} ktrace_event_s;

typedef const ktrace_event_s* ktrace_event_t;

/* Wall time information */
typedef struct {
    uint64_t walltime;
} ktrace_event_walltime_s;
typedef const ktrace_event_walltime_s* ktrace_event_walltime_t;

/* Core API */
extern ktrace_session_t ktrace_session_create(void);
extern void ktrace_session_destroy(ktrace_session_t s);

extern int ktrace_set_completion_handler(ktrace_session_t s, void (^handler)(void));
extern int ktrace_set_dropped_events_handler(ktrace_session_t s, void (^handler)(void));

typedef void (^ktrace_events_block_t)(ktrace_event_t event);

extern int ktrace_events_single(ktrace_session_t s, uint32_t debugid, ktrace_events_block_t handler);
extern int ktrace_events_single_paired(ktrace_session_t s, uint32_t debugid, ktrace_events_block_t handler);
extern int ktrace_events_class(ktrace_session_t s, uint8_t cls, ktrace_events_block_t handler);
extern int ktrace_events_subclass(ktrace_session_t s, uint8_t cls, uint8_t subclass, ktrace_events_block_t handler);
extern int ktrace_events_range(ktrace_session_t s, uint32_t start, uint32_t end, ktrace_events_block_t handler);
extern int ktrace_events_all(ktrace_session_t s, ktrace_events_block_t handler);

extern int ktrace_filter_pid(ktrace_session_t s, pid_t pid);
extern int ktrace_exclude_pid(ktrace_session_t s, pid_t pid);

extern int ktrace_set_use_existing(ktrace_session_t s);
extern int ktrace_set_file(ktrace_session_t s, const char *path);

extern int ktrace_start(ktrace_session_t s, dispatch_queue_t queue);
extern void ktrace_end(ktrace_session_t s, bool flush);

extern int ktrace_set_execnames_enabled(ktrace_session_t s, uint32_t flag);
extern int ktrace_set_vnode_paths_enabled(ktrace_session_t s, uint32_t flag);
extern int ktrace_set_walltime_enabled(ktrace_session_t s, uint32_t flag);

typedef void (^ktrace_thread_exit_block_t)(uint64_t threadid, const char *execname);
extern int ktrace_set_thread_exit_handler(ktrace_session_t s, ktrace_thread_exit_block_t handler);

extern const char* ktrace_get_execname_for_thread(ktrace_session_t s, uint64_t threadid);
extern int ktrace_exclude_process(ktrace_session_t s, const char *name);
extern int ktrace_filter_process(ktrace_session_t s, const char *name);
extern int ktrace_set_signal_handler(ktrace_session_t s);
extern int ktrace_set_default_event_filter(ktrace_session_t s);

#define KTRACE_FEATURE_LAZY   0x01
#define KTRACE_FEATURE_ENABLED  0x02
#define KTRACE_FEATURE_DISABLED 0x00
extern int ktrace_set_features(ktrace_session_t s, uint32_t features);
extern int ktrace_set_uuid_map_enabled(ktrace_session_t s, uint32_t flag);

extern int ktrace_convert_timestamp_to_nanoseconds(ktrace_session_t s, uint64_t timestamp, uint64_t *nanoseconds);
extern pid_t ktrace_get_pid_for_thread(ktrace_session_t s, uint64_t threadid);

#ifdef __cplusplus
}
#endif

#endif
