#ifndef T4C_PARAMETERS_INCLUDED
#define T4C_PARAMETERS_INCLUDED

#include <t4c/string.h>

typedef struct _parameter {
  string key,
         value;
} parameter;
typedef struct _list list;

list new_parameters();

void add_parameter(list* params, string key, string value);

void free_parameters(list* params);

string join_parameters(list* params, char* separator);
#endif
