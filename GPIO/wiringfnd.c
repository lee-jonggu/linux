#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

int fndControl(int num)
{
    int i, a, b;
    int gpiopins[7] = {25,24,23,22,21,29,28};
    int sevenseg [10][7] = { {1, 1, 1, 1, 1, 1, 0},
                             {0, 1, 1, 0, 0, 0, 0},
                             {1, 1, 0, 1, 1, 0, 1},
                             {1, 1, 1, 1, 0, 0, 1},
                             {0, 1, 1, 0, 0, 1, 1 },
                             {1, 0, 1, 1, 0, 1, 1},
                             {1, 0, 1, 1, 1, 1, 1},
                             {1, 1, 1, 0, 0, 0, 0},
                             {1, 1, 1, 1, 1, 1, 1},
                             {1, 1, 1, 0, 0, 1, 1} };

    for (i=0; i<7; i++) {
        pinMode(gpiopins[i], OUTPUT);
    }

    for (i=0; i<7; i++) {
        digitalWrite(gpiopins[i], sevenseg[num][i]?HIGH:LOW);
    }
    getchar();

    for (i=0; i<7; i++) {
        digitalWrite(gpiopins[i], HIGH);
    }
    return 0;
}

int main(int argc, char **argv)
{
    int no;

    if(argc < 2) {
        printf("Usage : %s NO\n", argv[0]);
        return -1;
    }

    no = atoi(argv[1]);
    wiringPiSetup();
    fndControl(no);

    return 0;
}
