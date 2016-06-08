#include <t4c/parameters.h>
#include <t4c/string.h>
#include <t4c/util.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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

    free(node->value);
    free(node);
  }
}

void free_parameters(Parameters* params) {
  free_all_parameter(params->firstNode);
  free(params);
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

void add_parameter(Parameters* params, string key, string value) {
  Parameter* param = MALLOC_T(Parameter);

  param->key   = key;
  param->value = value;

  add_param(params, param);
}

static string join_parameter_key_and_value(Parameter* param, char* separator) {
  string result = new_string();
  result.length = param->key.length + strlen(separator) + param->value.length;
  result.value  = MALLOC_TN(char, result.length);
  sprintf(result.value, "%s%s%s", param->key.value, separator, param->value.value);

  return result;
}

string join_parameters(Parameters* params, char* separator) {
  string str = new_string();
//  if (params == NULL || is_parameters_empty(params)) return str;

  Node* thisNode;

  for (thisNode = params->firstNode; thisNode != NULL; thisNode = thisNode->next) {  
    string joined = join_parameter_key_and_value(thisNode->value, "=");
    str.length += joined.length;

    if (thisNode != params->firstNode) {
      str.length += strlen(separator);// for "&"
    }
  }

  str.value = MALLOC_TN(char, str.length);

  for (thisNode = params->firstNode; thisNode != NULL; thisNode = thisNode->next) {  
    if (thisNode != params->firstNode) {
      strcat(str.value, separator);
    }
    string joined = join_parameter_key_and_value(thisNode->value, "=");
    strcat(str.value, string_get_value(joined));
  }

  return str;
}
