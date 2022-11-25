#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "bmpHeader.h"

int readBmp(char *filename, unsigned char **data, int *cols, int *rows)
{
    BITMAPFILEHEADER bmpHeader;
    BITMAPINFOHEADER bmpInfoHeader; 
    unsigned char ID[2]; 
    FILE *fp;

    /* BMP 파일을 오픈한다. */
    fp = fopen(filename,"rb");
    if(fp == NULL) {
        printf("ERROR\n");
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fread(&bmpHeader.bfType, sizeof(unsigned short int), 1, fp);
    fread(&bmpHeader.bfSize, sizeof(unsigned int), 1, fp);
    fread(&bmpHeader.bfReserved1, sizeof(unsigned short int), 1, fp);
    fread(&bmpHeader.bfReserved2, sizeof(unsigned short int), 1, fp);
    fread(&bmpHeader.bf0ffBits, sizeof(unsigned int), 1, fp);

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fread(&bmpInfoHeader.biSize, sizeof(unsigned int), 1, fp);
    fread(&bmpInfoHeader.biWidth, sizeof(unsigned int), 1, fp);
    fread(&bmpInfoHeader.biHeight, sizeof(unsigned int), 1, fp);
    fread(&bmpInfoHeader.biPlanes, sizeof(unsigned short int), 1, fp);
    fread(&bmpInfoHeader.biBitCount, sizeof(unsigned short int), 1, fp);
    fread(&bmpInfoHeader.biCompression, sizeof(unsigned int), 1, fp);
    fread(&bmpInfoHeader.SizeImage, sizeof(unsigned int), 1, fp);
    fread(&bmpInfoHeader.biXpelsPerMeter, sizeof(unsigned int), 1, fp);
    fread(&bmpInfoHeader.biYpelsPerMeter, sizeof(unsigned int), 1, fp);
    fread(&bmpInfoHeader.biClrUsed, sizeof(unsigned int), 1, fp);
    fread(&bmpInfoHeader.biClrImportant, sizeof(unsigned int), 1, fp);

    /* 트루 칼러를 지원하지 않으면 표시할 수 없다. */
    if(bmpInfoHeader.biBitCount != 24) {
       printf("This image file doesn’t supports 24bit color\n");
       fclose(fp);
       return -1;
    }

    /* 이미지에서 해상도의 정보를 가져온다. */
    *cols = bmpInfoHeader.biWidth;
    *rows = bmpInfoHeader.biHeight;
    printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
    printf("Bit Count : %d\n", bmpInfoHeader.biBitCount);
    printf("Size : %d\n", bmpHeader.bfSize);

    /* 실제 이미지 데이터가 있는 위치를 계산해서 가져온다. */
    fseek(fp, bmpHeader.bf0ffBits, SEEK_SET);
    fread(*data, 1, bmpHeader.bfSize-bmpHeader.bf0ffBits, fp);

    fclose(fp);                                              /* 사용이 끝난 이미지 파일을 닫는다. */

    return 0;
}

