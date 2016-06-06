#include <t4c/string.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

string hmac_sha1(string key, string data){
  string result = new_string(),
         res    = new_string();
  res.length = SHA_DIGEST_LENGTH + 1;
  res.value  = (char*)malloc(sizeof(char) * res.length);

  HMAC(EVP_sha1(), 
      (const unsigned char*)string_get_value(key), string_length(key),
      (const unsigned char*)string_get_value(data), string_length(data),
      (unsigned char*)res.value, (unsigned int*)&res.length);

  return res;
}


