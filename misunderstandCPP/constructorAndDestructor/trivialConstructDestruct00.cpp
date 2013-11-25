#include <cstdio>

struct trivialStruct
{

int a;
float b;
float c;

};

int main()
{
  trivialStruct A;  
  printf("%d, %f, %f", A.a, A.b, A.c);

  return 0;
}