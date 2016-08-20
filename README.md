# T4C
A Twitter API Wrapper Library for C.  
  
  
# Features
* REST APIs(GET/POST) Support
* Streaming APIs Support
  
  

# Rquirements
* libcurl
* openssl
  
  
# Documents
  
You should include for using t4c these headers:
* t4c/parameters.h
* t4c/string.h
* t4c/t4c.h
* sds/sds.h

```c
#include <t4c/parameters.h>
#include <t4c/string.h>
#include <t4c/t4c.h>
#include <sds/sds.h>
```
  
#At first, Define and Initialize an instance of T4C

```c
T4C t4c = {
  .consumerKey       = sdsnew("Your Consumer Key"),
  .consumerSecret    = sdsnew("Your Consumer Secret"),
  .accessToken       = sdsnew("Your AccessToken"),
  .accessTokenSecret = sdsnew("Your AccessTokenSecret")
};
```
  
This Library provides only manual way to access Twitter API Like [Twitter4D](https://github.com/alphaKAI/Twitter4D).  
This means that t4c doesn't provides functions for each of the APIs, only request function.  
  
#Parameters
Preparing parameters for each of the API gose like:
##1. Define and Initialize parameters

```c
Parameters* params = new_parameters();
```

##2. Add parameter's key and that of value

```c
add_parameter(params, sdsnew("Key"), sdsnew("Value"));
```

##If the parameters are unneeded, you should free as follows

```c
free_parameters(params);
```

#REST APIs(POST/GET)
Using `request` function.  
Function prototype declaration is:

```c
string request(T4C* t4c, METHOD method, string endPoint, Parameters* paramsArgument);
/*
Parameters:
  t4c: pointer for an instance of T4C
  method: specification whether the request is POST or GET
  endPoint: specification of endPoint(Ex: /foo/bar.json)
  paramsArgument: parameters for the endPoint
Returns:
  string of raw Twitter returned JSON
  If you need returned value, you must use some JSON parser(This library doesn't serve it)
*/
```

##POST request. for example: statuses/update.json

```c
Parameters* params = new_parameters();

add_parameter(params, sdsnew("status"), sdsnew("Hello World!"));

string result = request(&t4c, POST, sdsnew("/statuses/update.json"), params);

printf("RESULT for Tweet: %s\n", string_get_value(result));

free_parameters(params);
```

##GET request. for example: account/verify\_credentials.json
This API doesn't require essential parameter.  

```c
string result = request(&t4c, GET, sdsnew("/account/verify_credentials.json"), NULL);

printf("RESULT for /account/verify_credentials.json: %s\n", string_get_value(result));
```

#Streaming API
You can use Streaming API, but must should prepare callback function for each result of Streaming API as libcurl callback, like:

```c
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
    fprintf(stderr, "[STREAMING API] received -> %s\n", str->value);
  }

  return realsize;
}
```

##UserStream user.json

```c
stream(&t4c, sdsnew("https://userstream.twitter.com/1.1/user.json"), NULL, streaming_callback_sample);
/*
  If any result from twitter recives, streaming_callback_sample is called.
*/
```

##Filtered Stream statuses/filter.json

```c
Parameters* params = new_parameters();

add_parameter(params, sdsnew("track"), sdsnew("foobar"));

stream(&t4c, sdsnew("https://stream.twitter.com/1.1/statuses/filter.json"), params, streaming_callback_sample);

free_parameters(params);
```

# Build
  `$ make static`: build a static library(`t4c.a`)  
  `$ make shared`: build a shared library(`t4c.so`)  
  `$ make test`: build a test binary(`t4c_test/t4c_test`) this job is equivalent to `$ make test_static` in directly `t4c_test/`  

# LICENSE
This library is relased under the MIT license. Pleas see the `LICENSE` for details.  
Copyright (C) alphaKAI 2016 http://alpha-kai-net.info  
  
This library includes [sds](https://github.com/antirez/sds).  
The library is relased under BSD two clause license. Plase see the `libs/sds/LICENSE` for details.  
