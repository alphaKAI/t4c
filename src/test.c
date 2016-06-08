#include <t4c/parameters.h>
#include <t4c/string.h>
#include <t4c/t4c.h>
#include <stdio.h>

int main() {
  T4C t4c = {};

  t4c.consumerKey       = make_string("Your Consumer Key");
  t4c.consumerSecret    = make_string("Your Consumer Secret");
  t4c.accessToken       = make_string("Your AccessToken");
  t4c.accessTokenSecret = make_string("Your AccessTokenSecret");

  Parameters* params = new_parameters();

  add_parameter(params, make_string("status"), make_string("Hello World!"));
  string result  = request(&t4c, POST, make_string("/statuses/update.json"), params);

  string result2 = request(&t4c, GET, make_string("/account/verify_credentials.json"), NULL);

  printf("RESULT for Tweet: %s\n", string_get_value(result));
  printf("RESULT for /account/verify_credentials.json: %s\n", string_get_value(result2));

  free_parameters(params);
  return 0;
}
