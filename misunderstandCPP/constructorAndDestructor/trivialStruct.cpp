#include <cstdlib>

#include "trivialStruct.h"

trivialStruct::trivialStruct() : a((int*)malloc(sizeof(int))), b((float*)malloc(sizeof(float))), c((float*)malloc(sizeof(float)))
{
  *a = 100;
  *b = 200;
  *c = 300;
}

trivialStruct::~trivialStruct()
{
  free(a);
  free(b);
  free(c);
  
  a = nullptr;
  b = nullptr;
  c = nullptr;
}