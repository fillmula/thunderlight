#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


uint32_t fast_hash(const char *data, int len);

#ifdef __cplusplus
}
#endif
