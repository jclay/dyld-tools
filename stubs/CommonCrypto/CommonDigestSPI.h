/* Stub for private CommonCrypto/CommonDigestSPI.h */
#ifndef _COMMONCRYPTO_COMMONDIGESTSPI_H_STUB
#define _COMMONCRYPTO_COMMONDIGESTSPI_H_STUB

#include <CommonCrypto/CommonDigest.h>

/* CCDigest SPI — used by dsc_extractor.cpp */
typedef uint32_t CCDigestAlgorithm;

#define kCCDigestSHA256 0
#define kCCDigestSHA384 1
#define kCCDigestSHA1   2
#define kCCDigestNone   3

static inline int CCDigest(CCDigestAlgorithm alg,
                            const uint8_t *data, size_t dataLen,
                            uint8_t *output) {
    switch (alg) {
        case kCCDigestSHA256: CC_SHA256(data, (CC_LONG)dataLen, output); return 0;
        case kCCDigestSHA384: CC_SHA384(data, (CC_LONG)dataLen, output); return 0;
        case kCCDigestSHA1:   CC_SHA1(data, (CC_LONG)dataLen, output); return 0;
    }
    return -1;
}

static inline size_t CCDigestGetOutputSize(CCDigestAlgorithm alg) {
    switch (alg) {
        case kCCDigestSHA256: return CC_SHA256_DIGEST_LENGTH;
        case kCCDigestSHA384: return CC_SHA384_DIGEST_LENGTH;
        case kCCDigestSHA1:   return CC_SHA1_DIGEST_LENGTH;
    }
    return 0;
}

#endif
