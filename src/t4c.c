#include <t4c/parameters.h>
#include <t4c/hmac_sha1.h>
#include <sds/sds.h>
#include <t4c/util.h>
#include <t4c/t4c.h>
#include <t4c/url.h>
#include <curl/curl.h>
#include <resolv.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sds/sds.h>

#define DEBUG true

static int k64_encode (sds data, sds buf) {
  return b64_ntop ((const unsigned char*)data, sdslen(data), buf, sdslen(buf));
}

static sds signature(sds consumerSecret, sds accessTokenSecret, METHOD method, sds url, Parameters *params) {
  sds query      = join_parameters(params, "&");
  sds encodedCS  = url_encode(consumerSecret);
  sds encodedATS = url_encode(accessTokenSecret);

  sds key = sdscatprintf(sdsempty(), "%s&%s", encodedCS, encodedATS);

  sds method_str;

  if (method == POST) {
    method_str = sdsnew("POST");
  } else {
    method_str = sdsnew("GET");
  }

  sds encodedURL    = url_encode(url);
  sds encodedQuery  = url_encode(query);
  sds base = sdscatprintf(sdsempty(), "%s&%s&%s", method_str, encodedURL, encodedQuery);
  sds res  = hmac_sha1(key, base);
  sds buf  = sdsgrowzero(sdsempty(), 256);

  k64_encode (res, buf);

  sdsfree(query);
  sdsfree(encodedCS);
  sdsfree(encodedATS);
  sdsfree(key);
  sdsfree(method_str);
  sdsfree(encodedURL);
  sdsfree(encodedQuery);
  sdsfree(base);
  sdsfree(res);

  return sdsnewlen(buf, strlen(buf));
}

static void genOAuthParams(T4C* t4c, Parameters* params) {
  time_t now = time(NULL);
  char*  now_str = MALLOC_TS(char, sizeof(now));
  sprintf(now_str, "%d", (int)now);

  add_parameter(params, sdsnew("oauth_consumer_key"),     t4c->consumerKey);
  add_parameter(params, sdsnew("oauth_nonce"),            sdsnew(now_str));
  add_parameter(params, sdsnew("oauth_signature_method"), sdsnew("HMAC-SHA1"));
  add_parameter(params, sdsnew("oauth_timestamp"),        sdsnew(now_str));
  add_parameter(params, sdsnew("oauth_token"),            t4c->accessToken);
  add_parameter(params, sdsnew("oauth_version"),          sdsnew("1.0"));

  free(now_str);
}

static void buildParams(Parameters* params, Parameters* oauthParams, Parameters* additionalParam) {
  for (Node* thisNode = oauthParams->firstNode; thisNode != NULL; thisNode = thisNode->next) {
    add_parameter(params, thisNode->value->key, thisNode->value->value);
  }

  if (additionalParam != NULL && !is_parameters_empty(additionalParam)) {
    Parameters* adParams = additionalParam;

    for (Node* thisNode = adParams->firstNode; thisNode != NULL; thisNode = thisNode->next) {
      add_parameter(params, thisNode->value->key, url_encode(thisNode->value->value));
    }
  }
}

static size_t curl_result_stringlize_callback(void* ptr, size_t size, size_t nmemb, void* data) {
  if (size * nmemb == 0)
    return 0;

  size_t realsize = size * nmemb;

  string* str = (string*)data;
  str->length = realsize;
  str->value  = MALLOC_TN(char, str->length);

  if (str->value != NULL) {
    memcpy(str->value, ptr, realsize);
  }

  return realsize;
}

sds request(T4C* t4c, METHOD method, sds endPoint, Parameters* paramsArgument) {
  sds result;

  Parameters* oauthParams = new_parameters();
  genOAuthParams(t4c, oauthParams);
  Parameters* params = new_parameters();
  buildParams(params, oauthParams, paramsArgument);

  sds url = sdscatprintf(sdsempty(), "%s%s", baseUrl, endPoint);
  sds oauthSignature   = signature(t4c->consumerSecret, t4c->accessTokenSecret, method, url, params);
  sds encodedSignature = url_encode(oauthSignature);

  add_parameter(oauthParams, sdsnew("oauth_signature"), encodedSignature);
  add_parameter(params,     sdsnew("oauth_signature"),  encodedSignature);

  sds authorizeChild = join_parameters(oauthParams, ",");
  sds authorize      = sdscatprintf(sdsempty(), "Authorization: OAuth %s", authorizeChild);

  sds path = join_parameters(params, "&");

  if (DEBUG) {
    printf("----------------------------\n");
    printf("URL: %s\n", url);
    printf("endPoint: %s\n", endPoint);
    printf("path: %s\n", path);
    printf("authorize: %s\n", authorize);
    printf("----------------------------\n");
  }

  CURL* curl;
  curl = curl_easy_init();

  sds reqURL;

  if (method == GET) {
    reqURL = sdscatprintf(sdsempty(), "%s?%s", url, path);
    curl_easy_setopt(curl, CURLOPT_URL, reqURL);
  } else if (method == POST) {
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, path);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, sdslen(path));
    curl_easy_setopt(curl, CURLOPT_URL, url);
  }

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, authorize);

  curl_easy_setopt(curl, CURLOPT_HEADER, headers);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_result_stringlize_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&result);

  curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  sdsfree(oauthSignature);
  sdsfree(encodedSignature);
  sdsfree(reqURL);
  sdsfree(url);
  sdsfree(path);
  sdsfree(authorize);
  sdsfree(authorizeChild);
  free_parameters(oauthParams);

  return result;
}

void stream(T4C* t4c, sds url, Parameters* paramsArgument, size_t (*callback)(void*, size_t, size_t, void*)) {
  Parameters* oauthParams = new_parameters();
  genOAuthParams(t4c, oauthParams);
  Parameters* params = new_parameters();
  buildParams(params, oauthParams, paramsArgument);

  sds oauthSignature   = signature(t4c->consumerSecret, t4c->accessTokenSecret, GET, url, params);
  sds encodedSignature = url_encode(oauthSignature);

  add_parameter(oauthParams, sdsnew("oauth_signature"), encodedSignature);
  add_parameter(params,      sdsnew("oauth_signature"), encodedSignature);

  sds authorizeChild = join_parameters(oauthParams, ",");
  sds authorize      = sdscatprintf(sdsempty(), "Authorization: OAuth %s", authorizeChild);

  sds path = join_parameters(params, "&");

  if (DEBUG) {
    printf("----------------------------\n");
    printf("STREAMING API");
    printf("URL: %s\n", url);
    printf("path: %s\n", path);
    printf("authorize: %s\n", authorize);
    printf("----------------------------\n");
  }

  CURL* curl;
  curl = curl_easy_init();

  sds reqURL = sdscatprintf(sdsempty(), "%s?%s", url, path);

  curl_easy_setopt(curl, CURLOPT_URL, reqURL);

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, authorize);

  curl_easy_setopt(curl, CURLOPT_HEADER, headers);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 0);

  curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  sdsfree(oauthSignature);
  sdsfree(encodedSignature);
  sdsfree(reqURL);
  sdsfree(url);
  sdsfree(path);
  sdsfree(authorize);
  sdsfree(authorizeChild);

  free_parameters(oauthParams);
}
