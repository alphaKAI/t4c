#ifndef T4C_INCLUDED
#define T4C_INCLUDED

#include <sds/sds.h>
#include <t4c/parameters.h>
#define baseUrl      "https://api.twitter.com/1.1"
#define oauthBaseUrl "https://api.twitter.com/oauth/"

typedef enum {
  POST,
  GET
} METHOD;

typedef struct {
  sds consumerKey, 
      consumerSecret,
      accessToken,
      accessTokenSecret;
} T4C;

sds request(T4C* t4c, METHOD method, sds endPoint, Parameters* paramsArgument);

void stream(T4C* t4c, sds url, Parameters* paramsArgument, size_t (*callback)(void*, size_t, size_t, void*));
#endif
