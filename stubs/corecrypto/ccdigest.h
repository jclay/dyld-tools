/* Wrapper: adds extern "C" linkage, then includes the real header from
 * xnu EXTERNAL_HEADERS/corecrypto/ (which lacks C++ guards). */
#ifndef _CCDIGEST_WRAPPER_H
#define _CCDIGEST_WRAPPER_H
#ifdef __cplusplus
extern "C" {
#endif
#include_next <corecrypto/ccdigest.h>
#ifdef __cplusplus
}
#endif
#endif
