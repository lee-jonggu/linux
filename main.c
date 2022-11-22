#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    printf("Hello, World!");
    fflush(NULL);
    _exit(0);
}
