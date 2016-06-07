#include <t4c/t4c.h>
#include <t4c/url.h>
#include <t4c/string.h>
#include <t4c/hmac_sha1.h>
#include <t4c/parameters.h>
#include <t4c/linkedlist/linkedList.h>
#include <curl/curl.h>
#include <resolv.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int k64_encode (string data, string buf) {
  return b64_ntop ((const unsigned char*)string_get_value(data), string_length(data), buf.value, buf.length);
}

static string signature(string consumerSecret, string accessTokenSecret, METHOD method, string url, list* params) {
  string query      = join_parameters(params, "&");
  string encodedCS  = url_encode(consumerSecret);
  string encodedATS = url_encode(accessTokenSecret);

  string key;
  key.length = encodedCS.length + 1 + encodedATS.length;
  key.value  = (char*)malloc(sizeof(char) * key.length);
  sprintf(key.value, "%s&%s", string_get_value(encodedCS), string_get_value(encodedATS));

  string base;

  char* method_str;

  if (method == POST) {
    method_str = "POST";
  } else {
    method_str = "GET";
  }

  string encodedMethod = make_string(method_str);
  string encodedURL    = url_encode(url);
  string encodedQuery  = url_encode(query);
  
  base.length = encodedMethod.length + 1 + encodedURL.length + 1 + encodedQuery.length;
  base.value  = (char*)malloc(sizeof(char) * base.length);
  sprintf(base.value, "%s&%s&%s", string_get_value(encodedMethod), string_get_value(encodedURL), string_get_value(encodedQuery));

  string res = hmac_sha1(key, base);
  string buf = new_string();

  buf.length = 256;
  buf.value  = (char*)malloc(sizeof(char) * buf.length);

  k64_encode (res, buf);

  return buf;
}

static list* genOAuthParams(T4C* t4c) {
  list* params = (list*)malloc(sizeof(list));

  time_t now = time(NULL);
  char*  now_str = (char*)malloc(sizeof(char) * sizeof(now));
  sprintf(now_str, "%d", (int)now);

  add_parameter(params, make_string("oauth_consumer_key"), t4c->consumerKey);
  add_parameter(params, make_string("oauth_nonce"), make_string(now_str));
  add_parameter(params, make_string("oauth_signature_method"), make_string("HMAC-SHA1"));
  add_parameter(params, make_string("oauth_timestamp"), make_string(now_str));
  add_parameter(params, make_string("oauth_token"), t4c->accessToken);
  add_parameter(params, make_string("oauth_version"), make_string("1.0"));

  return params;
}

static list* buildParams(list* oauthParams, list* additionalParam) {
  list* params = (list*)malloc(sizeof(list));

  for(oauthParams->thisNode = oauthParams->firstNode; oauthParams->thisNode != NULL;
      oauthParams->thisNode = oauthParams->thisNode->nextNode) {
    add_parameter(params, oauthParams->thisNode->value.key, oauthParams->thisNode->value.value);
  }

  if (additionalParam != NULL) {
    list* adParams = additionalParam;

    for(adParams->thisNode = adParams->firstNode; adParams->thisNode != NULL;
      adParams->thisNode = adParams->thisNode->nextNode) {
      string v = adParams->thisNode->value.value;
      add_parameter(params, adParams->thisNode->value.key, url_encode(v));
    }
  }
  
  return params;
}

string request(T4C* t4c, METHOD method, string endPoint, list* paramsArgument) {
  string result;

  list* oauthParams = genOAuthParams(t4c);
  list* params = buildParams(oauthParams, paramsArgument);

  string url = new_string();
  url.length = strlen(baseUrl) + string_length(endPoint);
  url.value  = (char*)malloc(sizeof(char) * url.length);
  sprintf(url.value , "%s%s", baseUrl, string_get_value(endPoint));

  string oauthSignature = signature(t4c->consumerSecret, t4c->accessTokenSecret, method, url, params);
  int len = 0;

  for (int i = 0; i < oauthSignature.length; i++) {
    if (oauthSignature.value[i] == '\0') {
      break;
    } else {
      len++;
    }
  }
  oauthSignature.length = len;

  string encodedSignature = url_encode(oauthSignature);

  add_parameter(oauthParams, make_string("oauth_signature"), encodedSignature);
  add_parameter(params, make_string("oauth_signature"), encodedSignature);

  string authorize      = new_string();
  string authorizeChild = join_parameters(oauthParams, ",");
  authorize.length      = 21 + authorizeChild.length;
  authorize.value       = (char*)malloc(sizeof(char) * authorize.length);
  sprintf(authorize.value, "Authorization: OAuth %s", string_get_value(authorizeChild));

  string path = join_parameters(params, "&");

  printf("----------------------------\n");
  printf("URL: %s\n", string_get_value(url));
  printf("endPoint: %s\n", string_get_value(endPoint));
  printf("path: %s\n", string_get_value(path));
  printf("authorize: %s\n", string_get_value(authorize));
  printf("----------------------------\n");

  //Currently, this program support only post request
  if (method == POST) {
    CURL* curl;
    curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, url.value);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, string_get_value(authorize));

    curl_easy_setopt(curl, CURLOPT_HEADER, headers);

    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, string_get_value(path));
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, path.length);

    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }

  free_string(url);
  free_string(authorize);
  free_parameters(oauthParams);
  free_parameters(params);

  return result;
}
