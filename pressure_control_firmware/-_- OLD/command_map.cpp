#include <Arduino.h>
#include "command_map.h"

void CommandMap::emplace(String str_in, FunctionPointer fp_in){
  string_vec[idx] = str_in;
  func_vec[idx] = fp_in;

  idx++;
};


void CommandMap::set_default(FunctionPointer fp_in){
  fun_default = fp_in;
};

CommandMap::FunctionPointer CommandMap::find(String str_in){
  for (int i=0; i<idx;i++){
    if (string_vec[i] == str_in){
      return func_vec[i];
    }
  }
  // If nothing is found, return the default
  return fun_default;
};











