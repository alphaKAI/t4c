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

```c
#include <t4c/parameters.h>
#include <t4c/string.h>
#include <t4c/t4c.h>
```
  
#At first, Define and Initialize an instance of T4C

```c
T4C t4c = {
  .consumerKey       = make_string("Your Consumer Key"),
  .consumerSecret    = make_string("Your Consumer Secret"),
  .accessToken       = make_string("Your AccessToken"),
  .accessTokenSecret = make_string("Your AccessTokenSecret")
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
add_parameter(params, make_string("Key"), make_string("Value"));
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

add_parameter(params, make_string("status"), make_string("Hello World!"));

string result = request(&t4c, POST, make_string("/statuses/update.json"), params);

printf("RESULT for Tweet: %s\n", string_get_value(result));

free_parameters(params);
```

##GET request. for example: account/verify\_credentials.json
This API doesn't require essential parameter.  

```c
string result = request(&t4c, GET, make_string("/account/verify_credentials.json"), NULL);

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
stream(&t4c, make_string("https://userstream.twitter.com/1.1/user.json"), NULL, streaming_callback_sample);
/*
  If any result from twitter recives, streaming_callback_sample is called.
*/
```

##Filtered Stream statuses/filter.json

```c
Parameters* params = new_parameters();

add_parameter(params, make_string("track"), make_string("foobar"));

stream(&t4c, make_string("https://stream.twitter.com/1.1/statuses/filter.json"), params, streaming_callback_sample);

free_parameters(params);
```

# LICENSE
The MIT LICENSE  
Copyright (C) alphaKAI 2016 http://alpha-kai-net.info
