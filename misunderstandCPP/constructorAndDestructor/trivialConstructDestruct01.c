#include <stdio.h>
#include <stdlib.h>

typedef struct
{

int *a;
float *b;
float *c;

}trivialStruct;

void construct_trivial_struct(trivialStruct *data)
{
  data->a = (int*)malloc(sizeof(int));
  data->b = (float*)malloc(sizeof(float));
  data->c = (float*)malloc(sizeof(float));
  
  *data->a = 100;
  *data->b = 200;
  *data->c = 300;
}

void destruct_trivial_struct(trivialStruct *data)
{
  free(data->a);
  free(data->b);
  free(data->c);
  
  data->a = NULL;
  data->b = NULL;
  data->c = NULL;
}

int main()
{
  trivialStruct A;
  construct_trivial_struct(&A);
  
  printf("%d, %f, %f", *A.a, *A.b, *A.c);
  
  destruct_trivial_struct(&A);

  return 0;
}