/* Stub for private corecrypto/ccdigest.h
 * Uses CommonCrypto as the actual backend for digest computations.
 */
#ifndef _CORECRYPTO_CCDIGEST_H_STUB
#define _CORECRYPTO_CCDIGEST_H_STUB

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <CommonCrypto/CommonDigest.h>

/* Digest info — identifies which algorithm to use */
struct ccdigest_info {
    size_t   output_size;
    size_t   state_size;   /* size of context struct */
    size_t   block_size;
    int      algorithm;    /* 0=SHA1, 1=SHA256, 2=SHA384 */
};

/* Maximum context size we need (CC_SHA512_CTX is the largest) */
#define CCDIGEST_MAX_CTX_SIZE 256

/* Declare a stack buffer for digest state */
#define ccdigest_di_decl(di, name) \
    uint8_t name[CCDIGEST_MAX_CTX_SIZE]

/* Initialize digest context */
static inline void ccdigest_init(const struct ccdigest_info *di, void *ctx) {
    memset(ctx, 0, CCDIGEST_MAX_CTX_SIZE);
    switch (di->algorithm) {
        case 0: CC_SHA1_Init((CC_SHA1_CTX*)ctx); break;
        case 1: CC_SHA256_Init((CC_SHA256_CTX*)ctx); break;
        case 2: CC_SHA384_Init((CC_SHA512_CTX*)ctx); break;
    }
}

/* Update digest */
static inline void ccdigest_update(const struct ccdigest_info *di, void *ctx,
                                    size_t len, const void *data) {
    switch (di->algorithm) {
        case 0: CC_SHA1_Update((CC_SHA1_CTX*)ctx, data, (CC_LONG)len); break;
        case 1: CC_SHA256_Update((CC_SHA256_CTX*)ctx, data, (CC_LONG)len); break;
        case 2: CC_SHA384_Update((CC_SHA512_CTX*)ctx, data, (CC_LONG)len); break;
    }
}

/* Finalize digest */
static inline void ccdigest_final(const struct ccdigest_info *di, void *ctx,
                                   void *digest) {
    switch (di->algorithm) {
        case 0: CC_SHA1_Final((unsigned char*)digest, (CC_SHA1_CTX*)ctx); break;
        case 1: CC_SHA256_Final((unsigned char*)digest, (CC_SHA256_CTX*)ctx); break;
        case 2: CC_SHA384_Final((unsigned char*)digest, (CC_SHA512_CTX*)ctx); break;
    }
}

/* Clear digest context */
static inline void ccdigest_di_clear(const struct ccdigest_info *di __attribute__((unused)),
                                      void *ctx) {
    memset(ctx, 0, CCDIGEST_MAX_CTX_SIZE);
}

/* One-shot digest */
static inline void ccdigest(const struct ccdigest_info *di,
                            size_t len, const void *data,
                            void *digest) {
    switch (di->algorithm) {
        case 0: CC_SHA1(data, (CC_LONG)len, (unsigned char*)digest); break;
        case 1: CC_SHA256(data, (CC_LONG)len, (unsigned char*)digest); break;
        case 2: CC_SHA384(data, (CC_LONG)len, (unsigned char*)digest); break;
    }
}

#endif
