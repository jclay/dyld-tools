/* Stub for private corecrypto/ccsha1.h */
#ifndef _CORECRYPTO_CCSHA1_H_STUB
#define _CORECRYPTO_CCSHA1_H_STUB

#include "ccdigest.h"

#define CCSHA1_OUTPUT_SIZE CC_SHA1_DIGEST_LENGTH

static inline const struct ccdigest_info* ccsha1_di(void) {
    static const struct ccdigest_info di = {
        CC_SHA1_DIGEST_LENGTH,
        sizeof(CC_SHA1_CTX),
        CC_SHA1_BLOCK_BYTES,
        0
    };
    return &di;
}

#endif
