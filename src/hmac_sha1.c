#include <t4c/util.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <sds/sds.h>

sds hmac_sha1(sds key, sds data){
  sds res = sdsempty();
  res = sdsgrowzero(res, SHA_DIGEST_LENGTH + 1);
  
  unsigned int len;

  HMAC(EVP_sha1(), 
      key, sdslen(key),
      (const unsigned char*)data, sdslen(data),
      (unsigned char*)res, &len);

  return res;
}


