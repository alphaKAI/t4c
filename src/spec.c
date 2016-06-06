#include <t4c/linkedlist/spec.h>
#include <t4c/string.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char* showElement(TYPE* element) {
  char* str = (char*)malloc(sizeof(char) * (string_length(element->key) + 1 + string_length(element->value)));

  sprintf(str, "%s=%s", string_get_value(element->key), string_get_value(element->value));

  return str;
}

bool compareElement(TYPE fstArg, TYPE sndArg){
  return 
    strcmp(string_get_value(fstArg.key), string_get_value(sndArg.key))
  && 
    strcmp(string_get_value(fstArg.value), string_get_value(sndArg.value));
}
