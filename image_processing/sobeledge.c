#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     /* USHRT_MAX 상수를 위해서 사용한다. */
#include <unistd.h>
#include <math.h>

#include "bmpHeader.h"

/* 이미지 데이터의 경계 검사를 위한 매크로 */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte;

int main(int argc, char** argv) 
{
    FILE* fp; 
    BITMAPFILEHEADER bmpHeader;             /* BMP FILE INFO */
    BITMAPINFOHEADER bmpInfoHeader;     /* BMP IMAGE INFO */
    RGBQUAD *palrgb;
    ubyte *inimg, *padimg, *outimg;
    int i,j,r,g,b,x, y, z,index,gray, imageSize;

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

    int padSize = (bmpInfoHeader.biWidth + 2) * elemSize;
    int addSize = (padSize + bmpInfoHeader.biHeight)*2;
    padimg = (ubyte*)malloc(sizeof(ubyte)*(imageSize + addSize));


    /* make padding image */
    memset(padimg, 0, sizeof(ubyte)*(imageSize + addSize));
    for(y = 0; y < bmpInfoHeader.biHeight-1; y++) {
        for(x = 0; x < bmpInfoHeader.biWidth * elemSize; x+=elemSize) {
            for(z = 0; z < elemSize; z++) {
                padimg[(x+elemSize)+(y+1)*padSize+z]=inimg[x+y*size+z];
            }
        }
    }

    for(y = 0; y < bmpInfoHeader.biHeight; y++) { 
        for(z = 0; z < elemSize; z++) {
            padimg[0+(y+1)*padSize+z]=inimg[0+y*size+z];
            padimg[padSize-elemSize+(y+1)*padSize+z]=inimg[size-elemSize+y*size+z];
        }
    }

    for(x = 0; x < bmpInfoHeader.biWidth*elemSize; x++) { 
        padimg[elemSize+x]=inimg[x];
        padimg[elemSize+x+(bmpInfoHeader.biHeight+1)*padSize]=inimg[x+(bmpInfoHeader.biHeight-1)*size];
    }

    for(z = 0; z < elemSize; z++) {
       padimg[z]=inimg[z];
       padimg[padSize-elemSize+z]=inimg[size-elemSize+z];
       padimg[(bmpInfoHeader.biHeight+1)*padSize+z]=inimg[(bmpInfoHeader.biHeight-1)*size+z];
       padimg[(bmpInfoHeader.biHeight+1)*padSize+padSize-elemSize+z]=inimg[(bmpInfoHeader.biHeight-1)*size+size-elemSize+z];
    }
    // gray scaled
	for(i = 0; i < bmpInfoHeader.biHeight+1; i++) {
		index = (bmpInfoHeader.biHeight-i) * padSize; 
		for(j = 0; j < padSize; j++) { 
			r = (float)padimg[index+3*j+2];
			g = (float)padimg[index+3*j+1];
			b = (float)padimg[index+3*j+0];
			gray=(r*0.3F)+(g*0.59F)+(b*0.11F);
			
			padimg[index+elemSize*j] = padimg[index+elemSize*j+1] = padimg[index+elemSize*j+2] = gray;
		};
	};

  // sobel edge
    float yKernel[3][3] = { {-1, 0, 1},
                           {-2,  0, 2},
                           {-1, 0, 1} };
    float xKernel[3][3] = { {1, 2, 1},
                           {0,  0, 0},
                           {-1, -2, -1} };
    int c = 0;

    memset(outimg, 0, sizeof(ubyte)*imageSize);
    for(y = 1; y < bmpInfoHeader.biHeight+2; y++) { 
        for(x = elemSize; x < padSize; x+=elemSize) {
            for(z = 0; z < elemSize; z++) {
                float xVal = 0.0;
                float yVal = 0.0;
                for(int i = -1; i < 2; i++) {
                    for(int j = -1; j < 2; j++) {
                        xVal += xKernel[i+1][j+1]*padimg[(x+i*elemSize)+(y+j)*padSize+z];
                        yVal += yKernel[i+1][j+1]*padimg[(x+i*elemSize)+(y+j)*padSize+z];
                        c = sqrt(xVal*xVal+yVal*yVal);
                    }
                }
                outimg[(x-elemSize)+(y-1)*size+z] = LIMIT_UBYTE(c);
            }
			//outimg[(x-elemSize)+(y-1)*padSize] = outimg[(x-elemSize)+(y-1)*padSize+1] = outimg[(x-elemSize)+(y-1)*padSize+2] = LIMIT_UBYTE(c);
        }
    }         
     
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
    //fwrite(padimg, sizeof(ubyte), imageSize, fp);
    fwrite(outimg, sizeof(ubyte), imageSize, fp);

    fclose(fp); 

    free(inimg); 
    free(outimg);
    free(padimg);
    
    return 0;
}
