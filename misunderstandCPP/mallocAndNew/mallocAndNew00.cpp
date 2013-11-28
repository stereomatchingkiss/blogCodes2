#include <cstdio>

int main()
{
    char *a = new char[1024];

    for(int i = 0; i != 1024; ++i){
        printf("%c", a[i]);
    }

    delete []a;

    return 0;
}