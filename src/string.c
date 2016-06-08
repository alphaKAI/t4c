#include <t4c/string.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char* string_get_value(string str) {
  char* char_str;

  char_str  = (char*)malloc(sizeof(char) * (str.length + 1));

  if (char_str != NULL) {
    memcpy(char_str, str.value, str.length);
    char_str[str.length] = '\0';
  }
  return char_str;
}

string new_string() {
  string new_str;

  new_str.length = 0;
  new_str.value  = NULL;

  return new_str;
}

string make_string(char* char_str) {
  string new_str = new_string();

  string_set_value(&new_str, char_str);

  return new_str;
}

string* make_string_ptr(char* char_str) {
  string* v = (string*)malloc(sizeof(string));
  *v = make_string(char_str);
  return v;
}

void free_string(string str) {
  str.length = 0;
  free(str.value);
}

bool string_set_value(string* str, char* char_str) {
  size_t length = strlen(char_str);
  str->length   = length;

  str->value  = (char*)malloc(sizeof(char) * (length + 1));

  if (str->value != NULL) {
    memcpy(str->value, char_str, length);
    str->value[str->length] ='\0';
    return true;
  } else {
    return false;
  }
}

