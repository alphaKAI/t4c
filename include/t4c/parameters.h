#ifndef T4C_PARAMETERS_INCLUDED
#define T4C_PARAMETERS_INCLUDED
#include <t4c/string.h>
#include <stdbool.h>

typedef struct {
  string key,
         value;
} Parameter;

typedef struct _node {
  Parameter* value;
  struct _node* next;
} Node;

typedef struct {
  Node* firstNode;
  size_t length;
} Parameters;

Parameters* new_parameters();

bool is_parameters_empty(Parameters* params);

void add_parameter(Parameters* params, string key, string value);

void free_parameters(Parameters* params);

string join_parameters(Parameters* params, char* separator);
#endif
