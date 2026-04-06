/* Stub for private corecrypto/ccsha2.h */
#ifndef _CORECRYPTO_CCSHA2_H_STUB
#define _CORECRYPTO_CCSHA2_H_STUB

#include "ccdigest.h"

#define CCSHA256_OUTPUT_SIZE CC_SHA256_DIGEST_LENGTH
#define CCSHA384_OUTPUT_SIZE CC_SHA384_DIGEST_LENGTH

static inline const struct ccdigest_info* ccsha256_di(void) {
    static const struct ccdigest_info di = {
        CC_SHA256_DIGEST_LENGTH,
        sizeof(CC_SHA256_CTX),
        CC_SHA256_BLOCK_BYTES,
        1
    };
    return &di;
}

static inline const struct ccdigest_info* ccsha384_di(void) {
    static const struct ccdigest_info di = {
        CC_SHA384_DIGEST_LENGTH,
        sizeof(CC_SHA512_CTX),
        CC_SHA384_BLOCK_BYTES,
        2
    };
    return &di;
}

#endif
