#include <t4c/parameters.h>
#include <t4c/string.h>
#include <t4c/util.h>
#include <t4c/t4c.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static size_t streaming_callback_sample(void* ptr, size_t size, size_t nmemb, void* data) {
  if (size * nmemb == 0)
    return 0;

  size_t realsize = size * nmemb;
  string* str = (string*)data;
  str->length = realsize + 1;
  str->value  = MALLOC_TN(char, str->length);

  if (str->value != NULL) {
    memcpy(str->value, ptr, realsize);
    strcat(str->value, "\0");

    fprintf(stderr, "RECIEVED: %ld bytes\n", realsize);
    fprintf(stderr, "[USER STREAM] received -> %s\n", str->value);
  }

  return realsize;
}

int main() {
  T4C t4c = {};

  t4c.consumerKey       = make_string("Your Consumer Key");
  t4c.consumerSecret    = make_string("Your Consumer Secret");
  t4c.accessToken       = make_string("Your AccessToken");
  t4c.accessTokenSecret = make_string("Your AccessTokenSecret");

  // POST REQUEST
  Parameters* params = new_parameters();
  add_parameter(params, make_string("status"), make_string("Hello World!"));
  string result  = request(&t4c, POST, make_string("/statuses/update.json"), params);

  printf("RESULT for Tweet: %s\n", string_get_value(result));

  // GET REQUEST
  string result2 = request(&t4c, GET, make_string("/account/verify_credentials.json"), NULL);

  printf("RESULT for /account/verify_credentials.json: %s\n", string_get_value(result2));

  free_parameters(params);

  //STREAM API TEST
  stream(&t4c, make_string("https://userstream.twitter.com/1.1/user.json"), streaming_callback_sample);
  return 0;
}
