#include <t4c/string.h>
#include <t4c/parameters.h>
#include <t4c/linkedlist/linkedList.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

list new_parameters() {
  list new_params = {};

  return new_params;
}

void add_parameter(list* params, string key, string value) {

  parameter new_param;
  new_param.key   = key;
  new_param.value = value;
  addNode(params, new_param);
}

void free_parameters(list* params) {
  freeList(params);
}

string join_parameters(list* params, char* separator) {
  string str = new_string();

  for(params->thisNode = params->firstNode; params->thisNode != NULL;
      params->thisNode = params->thisNode->nextNode) {
    if (params->thisNode == params->firstNode) {
      str.length += strlen(showElement(&params->thisNode->value));
    } else {    
      str.length += strlen(showElement(&params->thisNode->value));
      str.length += strlen(separator);// for "&"
    }
  }

  str.value = (char*)malloc(sizeof(char) * str.length);

  for(params->thisNode = params->firstNode; params->thisNode != NULL;
      params->thisNode = params->thisNode->nextNode) {
    if (params->thisNode == params->firstNode) {
      strcat(str.value, showElement(&params->thisNode->value));
    } else {    
      strcat(str.value, separator);
      strcat(str.value, showElement(&params->thisNode->value));
    }
  }

  return str;
}
