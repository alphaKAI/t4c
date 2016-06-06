#include <t4c/t4c.h>
#include <t4c/string.h>
#include <t4c/parameters.h>
#include <t4c/linkedlist/linkedList.h>

int main() {
  T4C t4c = {};

  t4c.consumerKey       = make_string("Your Consumer Key");
  t4c.consumerSecret    = make_string("Your Consumer Secret");
  t4c.accessToken       = make_string("Your AccessToken");
  t4c.accessTokenSecret = make_string("Your AccessTokenSecret");

  list param = {};
  add_parameter(&param, make_string("status"), make_string("Hello World!"));

  request(&t4c, POST, make_string("/statuses/update.json"), &param);

  free_parameters(&param);
  return 0;
}
