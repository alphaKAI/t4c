#ifndef T4C_INCLUDED
#define T4C_INCLUDED

#include <t4c/string.h>
#include <t4c/parameters.h>
#define baseUrl      "https://api.twitter.com/1.1"
#define oauthBaseUrl "https://api.twitter.com/oauth/"

typedef enum {
  POST,
  GET
} METHOD;

typedef struct {
  string consumerKey, 
         consumerSecret,
         accessToken,
         accessTokenSecret;
} T4C;

string request(T4C* t4c, METHOD method, string endPoint, Parameters* paramsArgument);

#endif
