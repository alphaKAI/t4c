#ifndef T4C_PARAMETERS_INCLUDED
#define T4C_PARAMETERS_INCLUDED
#include <stdbool.h>
#include <sds/sds.h>

typedef struct {
  sds key,
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

void add_parameter(Parameters* params, sds key, sds value);

void free_parameters(Parameters* params);

sds join_parameters(Parameters* params, char* separator);
#endif
