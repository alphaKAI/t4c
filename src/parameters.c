#include <t4c/parameters.h>
#include <t4c/util.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sds/sds.h>

Parameters* new_parameters() {
  Parameters* newList = MALLOC_T(Parameters);

  newList->firstNode  = NULL;

  return newList;
}

void free_all_parameter(Node* node) {
  if (node != NULL) {
    if (node->next != NULL) {
      free_all_parameter(node->next);
    }

    free(node);
    node = NULL;
  }
}

void free_parameters(Parameters* params) {
  if (params != NULL) {
    if (params->firstNode != NULL) {
      free_all_parameter(params->firstNode);
    }

    free(params);
    params = NULL;
  }
}

Node* get_last_node(Parameters* params) {
  Node* thisNode;

  for (thisNode = params->firstNode; thisNode->next != NULL; thisNode = thisNode->next) {}

  return thisNode;
}

bool is_parameters_empty(Parameters* params) {
  if (params->firstNode == NULL) {
    return true;
  } else {
    return false;
  }
}

void add_node(Parameters* params, Node* node) {
  node->next = NULL;

  if (is_parameters_empty(params) || params->firstNode == NULL) {
    params->firstNode = node;
  } else {
    Node* lastNode = get_last_node(params);
    lastNode->next = node;
  }
}

void add_param(Parameters* params, Parameter* param) {
  Node* newNode  = MALLOC_T(Node);
  newNode->value = param;

  add_node(params, newNode);
}

void add_parameter(Parameters* params, sds key, sds value) {
  Parameter* param = MALLOC_T(Parameter);

  param->key   = key;
  param->value = value;

  add_param(params, param);
}

static sds join_parameter_key_and_value(Parameter* param, char* separator) {
  sds result = sdscatprintf(sdsempty(), "%s%s%s", param->key, separator, param->value);

  return result;
}

sds join_parameters(Parameters* params, char* separator) {
  if (params == NULL || is_parameters_empty(params)) return sdsempty();

  Node* thisNode;

  sds str = sdsempty();

  for (thisNode = params->firstNode; thisNode != NULL; thisNode = thisNode->next) {
    sds joined = join_parameter_key_and_value(thisNode->value, "=");
    if (thisNode != params->firstNode) {
      str = sdscatprintf(sdsempty(), "%s%s%s", str, separator, joined);
    } else {
      str = sdscatprintf(sdsempty(), "%s", joined);
    }
  }

  return str;
}
