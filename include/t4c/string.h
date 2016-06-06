#ifndef T4C_STRING_INCLUDED
#define T4C_STRING_INCLUDED

#include <stdbool.h>
#include <stddef.h>

typedef struct {
  char* value;
  size_t length;
} string;

size_t string_length(string str);

char* string_get_value(string str);

string new_string(); 

string make_string(char* char_str);

void free_string(string str);

bool string_set_value(string* str, char* char_str);
#endif
