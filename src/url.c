#include <t4c/string.h>
#include <t4c/util.h>
#include <t4c/url.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

string url_encode(string s) {
  string result;

  int k, enn = 0;
  for(k = 0; k < s.length; ++k){
    if(url_unreserved(s.value[k])) enn += 1;
    else enn += 3;
  }
  char *en = MALLOC_TN(char, (enn + 1));
  int ofst = 0;
  for(k = 0; k < s.length; ++k){
    if(url_unreserved(s.value[k])) en[ofst++] = s.value[k];
    else ofst += sprintf(en + ofst, "%%%02X", (unsigned char)s.value[k]);
  }
  en[ofst] = '\0';

  result = make_string(en);

  return result;
}

int url_unreserved(char c){
   if('a' <= c && c <= 'z') return 1;
   if('0' <= c && c <= '9') return 1;
   if('A' <= c && c <= 'Z') return 1;
   if(c == '-' || c == '.' || c == '_' || c == '~') return 1;
   return 0;
}
