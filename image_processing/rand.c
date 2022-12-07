#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main()
{
        srand((unsigned int)time(NULL));
        printf("random : %d\n", rand());
        printf("random : %d\n", rand());
        printf("random : %d\n", rand());
        printf("random : %d\n", rand());
        printf("random : %d\n", rand());
        printf("random : %d\n", rand());
        return 0;
}

