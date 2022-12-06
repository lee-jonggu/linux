#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     /* USHRT_MAX 상수를 위해서 사용한다. */
#include <unistd.h>
#include <time.h>

#include "bmpHeader.h"

/* 이미지 데이터의 경계 검사를 위한 매크로 */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte;

unsigned char clip(int value, int min, int max)
{
    return(value > max? max : value < min? min : value);
}

int main(int argc, char** argv) 
{
    FILE* fp; 
    BITMAPFILEHEADER bmpHeader;             /* BMP FILE INFO */
    BITMAPINFOHEADER bmpInfoHeader;     /* BMP IMAGE INFO */
    RGBQUAD *palrgb;
    ubyte *inimg, *outimg, *padimg;
    int i,j,x, y, z, imageSize, padsize;

    if(argc != 3) {
        fprintf(stderr, "usage : %s input.bmp output.bmp\n", argv[0]);
        return -1;
    }
    
    /***** read bmp *****/ 
    if((fp=fopen(argv[1], "rb")) == NULL) { 
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

    /* 트루 컬러를 지원하면 변환할 수 없다. */
    if(bmpInfoHeader.biBitCount != 24) {
        perror("This image file doesn't supports 24bit color\n");
        fclose(fp);
        return -1;
    }
    
    int elemSize = bmpInfoHeader.biBitCount/8;
    int size = bmpInfoHeader.biWidth*elemSize;
    imageSize = size * bmpInfoHeader.biHeight; 

    /* 이미지의 해상도(넓이 × 깊이) */
    printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
    printf("Bit Count : %d\n", bmpInfoHeader.biBitCount);     /* 픽셀당 비트 수(색상) */
    printf("Image Size : %d\n", imageSize);

    inimg = (ubyte*)malloc(sizeof(ubyte)*imageSize); 
    outimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);
    fread(inimg, sizeof(ubyte), imageSize, fp); 
    
    fclose(fp);
	for(i=0; i<bmpInfoHeader.biHeight*3; i+=3) { 
		for(j=0; j<bmpInfoHeader.biWidth*3; j+=3) {
			int b = inimg[j+(i*bmpInfoHeader.biWidth+0)]; // unsigned char 0 ~ 255
			int g = inimg[j+(i*bmpInfoHeader.biWidth+1)]; 
			int r = inimg[j+(i*bmpInfoHeader.biWidth+2)]; 
			
			outimg[j+bmpInfoHeader.biWidth*i+0]= clip(b, 0, 255);
			outimg[j+bmpInfoHeader.biWidth*i+1]= clip(g, 0, 255);
			outimg[j+bmpInfoHeader.biWidth*i+2]= clip(r, 0, 255);
		};
	 };	   

    srand((unsigned int)time(NULL));
    int count = 10000;
    for (int c = 0; c < count ; c++) {

            int ypos = rand()%(bmpInfoHeader.biWidth*bmpInfoHeader.biHeight);
            int xpos = rand()%(bmpInfoHeader.biWidth*bmpInfoHeader.biHeight);
            //printf("xpos : %d, ypos : %d\n",xpos,ypos);

            int noise = rand();
			int b = inimg[xpos*elemSize+ypos*elemSize] + noise; // unsigned char 0 ~ 255
			int g = inimg[xpos*elemSize+ypos*elemSize+1] + noise; 
			int r = inimg[xpos*elemSize+ypos*elemSize+2] + noise; 
			
//			b += noise;
//			g += noise;
//			r += noise; 
			
			outimg[xpos*elemSize+(ypos*elemSize)+0]= clip(b, 0, 255);
			outimg[xpos*elemSize+(ypos*elemSize)+1]= clip(g, 0, 255);
			outimg[xpos*elemSize+(ypos*elemSize)+2]= clip(r, 0, 255);
            //printf("RGB : %d %d %d\n",r,g,b);
            //printf("xpos : %d, ypos : %d\n",xpos*elemSize,ypos*elemSize);
    };

    /***** write bmp *****/ 
    if((fp=fopen(argv[2], "wb"))==NULL) { 
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

    //fwrite(inimg, sizeof(ubyte), imageSize, fp); 
    fwrite(outimg, sizeof(ubyte), imageSize, fp);

    fclose(fp); 
    
    free(inimg); 
    free(outimg);
    
    return 0;
}
