/* Stub for private _simple.h
 * Provides a minimal implementation of the _simple string buffer API.
 * The real API uses opaque void* handles. This stub backs them with
 * heap-allocated C string buffers.
 */
#ifndef _SIMPLE_H_STUB
#define _SIMPLE_H_STUB

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* The real _simple.h uses void* as the opaque handle type */
typedef void* _SIMPLE_STRING;

#ifdef __cplusplus
extern "C" {
#endif

/* Internal layout — users only see void* */
struct _simple_buf {
    char*  data;
    size_t capacity;
    size_t length;     /* current insertion point */
};

static inline _SIMPLE_STRING _simple_salloc(void) {
    struct _simple_buf* s = (struct _simple_buf*)calloc(1, sizeof(struct _simple_buf));
    if (s) {
        s->capacity = 256;
        s->data = (char*)calloc(1, s->capacity);
        s->length = 0;
    }
    return (_SIMPLE_STRING)s;
}

static inline void _simple_sfree(_SIMPLE_STRING handle) {
    struct _simple_buf* s = (struct _simple_buf*)handle;
    if (s) {
        free(s->data);
        free(s);
    }
}

static inline const char* _simple_string(_SIMPLE_STRING handle) {
    struct _simple_buf* s = (struct _simple_buf*)handle;
    return (s && s->data) ? s->data : "";
}

/* _simple_vsprintf appends formatted text at the current insertion point */
static inline void _simple_vsprintf(_SIMPLE_STRING handle, const char *fmt, va_list ap) {
    struct _simple_buf* s = (struct _simple_buf*)handle;
    if (!s) return;
    va_list ap2;
    va_copy(ap2, ap);
    int needed = vsnprintf(NULL, 0, fmt, ap2);
    va_end(ap2);
    if (needed < 0) return;
    size_t required = s->length + (size_t)needed + 1;
    if (required > s->capacity) {
        size_t newcap = required * 2;
        char* newbuf = (char*)realloc(s->data, newcap);
        if (!newbuf) return;
        s->data = newbuf;
        s->capacity = newcap;
    }
    vsnprintf(s->data + s->length, s->capacity - s->length, fmt, ap);
    s->length += (size_t)needed;
}

/* _simple_sprintf — variadic wrapper */
static inline void _simple_sprintf(_SIMPLE_STRING handle, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    _simple_vsprintf(handle, fmt, ap);
    va_end(ap);
}

/* _simple_sresize — moves the insertion point to the end of the current string */
static inline void _simple_sresize(_SIMPLE_STRING handle) {
    struct _simple_buf* s = (struct _simple_buf*)handle;
    if (s && s->data) {
        s->length = strlen(s->data);
    }
}

/* _simple_vsnprintf — just forwards to vsnprintf */
static inline int _simple_vsnprintf(char *buf, size_t size, const char *fmt, va_list ap) {
    return vsnprintf(buf, size, fmt, ap);
}

static inline int _simple_snprintf(char *buf, size_t size, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int r = vsnprintf(buf, size, fmt, ap);
    va_end(ap);
    return r;
}

/* _simple_dprintf — formatted print to file descriptor */
static inline void _simple_dprintf(int fd, const char *fmt, ...) {
    char buf[4096];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (n > 0) write(fd, buf, (size_t)n);
}

static inline void _simple_vdprintf(int fd, const char *fmt, va_list ap) {
    char buf[4096];
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    if (n > 0) write(fd, buf, (size_t)n);
}

/* _simple_getenv — scan an envp/apple array for key=value */
static inline const char* _simple_getenv(const char **envp, const char *key) {
    if (!envp || !key) return NULL;
    size_t klen = strlen(key);
    for (const char **p = envp; *p; ++p) {
        if (strncmp(*p, key, klen) == 0 && (*p)[klen] == '=')
            return *p + klen + 1;
    }
    return NULL;
}

#ifdef __cplusplus
}
#endif

#endif
