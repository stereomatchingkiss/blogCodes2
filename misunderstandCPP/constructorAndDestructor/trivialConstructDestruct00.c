#include <stdio.h>

struct trivialStruct
{

int a;
float b;
double c;

};

int main()
{
  struct trivialStruct A;
  printf("%d, %f, %f", A.a, A.b, A.c);

  return 0;
}