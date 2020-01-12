#include <stdlib.h>

#ifndef __command_map_H__
#define __command_map_H__


class CommandMap
{
  typedef void (&FunctionPointer)(); // function pointer type
  private:
    String string_vec[128]={};
    FunctionPointer func_vec[128]={} ;
    FunctionPointer fun_default;
    int idx = 0;
  public:
    CommandMap(){};
    void emplace(String, FunctionPointer);
    void set_default(FunctionPointer);
    FunctionPointer find(String);

};

#endif
