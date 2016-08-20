#ifndef T4C_URL_INCLUDED
#define T4C_URL_INCLUDED
#include <sds/sds.h>
sds url_encode(sds s);

int url_unreserved(char c);
#endif
