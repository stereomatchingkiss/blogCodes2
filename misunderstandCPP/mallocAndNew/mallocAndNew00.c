#include <stdio.h>
#include <stdlib.h> // pulls in declaration of malloc, free


int main()
{
    char *a = (char*)malloc(1024);

    for(int i = 0; i != 1024; ++i){
        printf("%c", a[i]);
    }

    free(a);
    a = NULL;

    return 0;
}