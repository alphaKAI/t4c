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
  T4C t4c = {
    .consumerKey       = make_string("Your Consumer Key"),
    .consumerSecret    = make_string("Your Consumer Secret"),
    .accessToken       = make_string("Your AccessToken"),
    .accessTokenSecret = make_string("Your AccessTokenSecret")
  };

  // POST REQUEST
  Parameters* params = new_parameters();

  add_parameter(params, make_string("status"), make_string("Hello World!"));

  string result  = request(&t4c, POST, make_string("/statuses/update.json"), params);
  printf("RESULT for Tweet: %s\n", string_get_value(result));

  free_parameters(params);

  // GET REQUEST
  string result2 = request(&t4c, GET, make_string("/account/verify_credentials.json"), NULL);
  printf("RESULT for /account/verify_credentials.json: %s\n", string_get_value(result2));

  //STREAM API TEST
  stream(&t4c, make_string("https://userstream.twitter.com/1.1/user.json"), NULL, streaming_callback_sample);
  
  /*
     Also supports Streaming API with parameters:

    Parameters* params = new_parameters();
    add_parameter(params, make_string("track"), make_string("foobar"));
    stream(&t4c, make_string("https://stream.twitter.com/1.1/statuses/filter.json"), params, streaming_callback_sample);
    free_parameters(params);
  */
  return 0;
}
