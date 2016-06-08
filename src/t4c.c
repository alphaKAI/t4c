#include <t4c/parameters.h>
#include <t4c/hmac_sha1.h>
#include <t4c/string.h>
#include <t4c/util.h>
#include <t4c/t4c.h>
#include <t4c/url.h>
#include <curl/curl.h>
#include <resolv.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WILL(x) fprintf(stderr, "WILL %s\n", x);
#define END(x) fprintf(stderr, "END %s\n", x);
#define DEBUG true
#define DS(x) fprintf(stderr, "%s\n", x)

static int k64_encode (string data, string buf) {
  return b64_ntop ((const unsigned char*)string_get_value(data), data.length, buf.value, buf.length);
}

static string signature(string consumerSecret, string accessTokenSecret, METHOD method, string url, Parameters *params) {
  string query      = join_parameters(params, "&");
  string encodedCS  = url_encode(consumerSecret);
  string encodedATS = url_encode(accessTokenSecret);

  string key;
  key.length = encodedCS.length + 1 + encodedATS.length;
  key.value  = MALLOC_TN(char, key.length);
  sprintf(key.value, "%s&%s", string_get_value(encodedCS), string_get_value(encodedATS));

  string base;

  string method_str;

  if (method == POST) {
    method_str = make_string("POST");
  } else {
    method_str = make_string("GET");
  }

  string encodedURL    = url_encode(url);
  string encodedQuery  = url_encode(query);
  
  base.length = method_str.length + 1 + encodedURL.length + 1 + encodedQuery.length;
  base.value  = MALLOC_TN(char, base.length);
  sprintf(base.value, "%s&%s&%s", string_get_value(method_str), string_get_value(encodedURL), string_get_value(encodedQuery));

  string res = hmac_sha1(key, base);
  string buf = new_string();

  buf.length = 256;
  buf.value  = MALLOC_TN(char, buf.length);

  k64_encode (res, buf);

  int len = 0;
  for (int i = 0; i < buf.length; i++) {
    if (buf.value[i] == '\0') {
      break;
    } else {
      len++;
    }
  }

  buf.length = len;

  return buf;
}

static void genOAuthParams(T4C* t4c, Parameters* params) {
  time_t now = time(NULL);
  char*  now_str = MALLOC_TS(char, sizeof(now));
  sprintf(now_str, "%d", (int)now);

  add_parameter(params, make_string("oauth_consumer_key"), t4c->consumerKey);
  add_parameter(params, make_string("oauth_nonce"), make_string(now_str));
  add_parameter(params, make_string("oauth_signature_method"), make_string("HMAC-SHA1"));
  add_parameter(params, make_string("oauth_timestamp"), make_string(now_str));
  add_parameter(params, make_string("oauth_token"), t4c->accessToken);
  add_parameter(params, make_string("oauth_version"), make_string("1.0"));

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

string request(T4C* t4c, METHOD method, string endPoint, Parameters* paramsArgument) {
  string result;

  bool paramsArgumentWasNULL = false;
  if (paramsArgument == NULL) {
    paramsArgument = new_parameters();
    paramsArgumentWasNULL = true;
  }

  Parameters* oauthParams = new_parameters();
  genOAuthParams(t4c, oauthParams);
  Parameters* params = new_parameters();
  buildParams(params, oauthParams, paramsArgument);

  string url = new_string();
  url.length = strlen(baseUrl) + endPoint.length;
  url.value  = MALLOC_TN(char, url.length);
  sprintf(url.value , "%s%s", baseUrl, string_get_value(endPoint));

  string oauthSignature = signature(t4c->consumerSecret, t4c->accessTokenSecret, method, url, params);
  string encodedSignature = url_encode(oauthSignature);

  add_parameter(oauthParams, make_string("oauth_signature"), encodedSignature);
  add_parameter(params, make_string("oauth_signature"), encodedSignature);

  string authorize      = new_string();
  string authorizeChild = join_parameters(oauthParams, ",");
  authorize.length      = 21 + authorizeChild.length;
  authorize.value       = MALLOC_TN(char, authorize.length);
  sprintf(authorize.value, "Authorization: OAuth %s", string_get_value(authorizeChild));

  string path = join_parameters(params, "&");

  if (DEBUG) {
    printf("----------------------------\n");
    printf("URL: %s\n", string_get_value(url));
    printf("endPoint: %s\n", string_get_value(endPoint));
    printf("path: %s\n", string_get_value(path));
    printf("authorize: %s\n", string_get_value(authorize));
    printf("----------------------------\n");
  }

  CURL* curl;
  curl = curl_easy_init();

  string reqURL = new_string();
  if (method == GET) {
    reqURL.length = url.length + 1 + path.length;
    reqURL.value  = MALLOC_TN(char, reqURL.length);
    sprintf(reqURL.value, "%s?%s", url.value, path.value);

    curl_easy_setopt(curl, CURLOPT_URL, reqURL.value);
  } else if (method == POST) {
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, string_get_value(path));
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, path.length);

    curl_easy_setopt(curl, CURLOPT_URL, url.value);
  }

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, string_get_value(authorize));
  curl_easy_setopt(curl, CURLOPT_HEADER, headers);

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_result_stringlize_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&result);

  curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  free_string(url);
  free_string(path);
  free_string(authorize);
  free_parameters(oauthParams);
  free_parameters(params);
  if (paramsArgumentWasNULL) {
    free_parameters(paramsArgument);
  }

  return result;
}

void stream(T4C* t4c, string url, size_t (*callback)(void*, size_t, size_t, void*)) {
  Parameters* oauthParams = new_parameters();
  genOAuthParams(t4c, oauthParams);
  Parameters* params = new_parameters();
  buildParams(params, oauthParams, NULL);

  string oauthSignature = signature(t4c->consumerSecret, t4c->accessTokenSecret, GET, url, params);
  string encodedSignature = url_encode(oauthSignature);

  add_parameter(oauthParams, make_string("oauth_signature"), encodedSignature);
  add_parameter(params, make_string("oauth_signature"), encodedSignature);

  string authorize      = new_string();
  string authorizeChild = join_parameters(oauthParams, ",");
  authorize.length      = 21 + authorizeChild.length;
  authorize.value       = MALLOC_TN(char, authorize.length);
  sprintf(authorize.value, "Authorization: OAuth %s", string_get_value(authorizeChild));

  string path = join_parameters(params, "&");

  if (DEBUG) {
    printf("----------------------------\n");
    printf("STREAMING API");
    printf("URL: %s\n", string_get_value(url));
    printf("path: %s\n", string_get_value(path));
    printf("authorize: %s\n", string_get_value(authorize));
    printf("----------------------------\n");
  }

  CURL* curl;
  curl = curl_easy_init();

  string reqURL = new_string();

  reqURL.length = url.length + 1 + path.length;
  reqURL.value  = MALLOC_TN(char, reqURL.length);
  sprintf(reqURL.value, "%s?%s", url.value, path.value);

  curl_easy_setopt(curl, CURLOPT_URL, reqURL.value);

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, string_get_value(authorize));
  curl_easy_setopt(curl, CURLOPT_HEADER, headers);

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 0);

  curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  free_string(url);
  free_string(path);
  free_string(authorize);
  free_parameters(oauthParams);
  free_parameters(params);
}
