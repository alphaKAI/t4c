#ifndef T4C_HMAC_SHA1_INCLUDED
#define T4C_HMAC_SHA1_INCLUDED

#include <t4c/string.h>
#include <sds/sds.h>
sds hmac_sha1(sds key, sds data);

#endif
