#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "bmpHeader.h"

#define BYTE unsigned char

#define widthbytes(bits) (((bits)+31)/32*4) // padding

inline unsigned char clip(int value, int min, int max);
unsigned char clip(int value, int min, int max)
{
    return(value > max? max : value < min? min : value);
}

int main(int argc, char** argv) {
    BITMAPFILEHEADER bmpFileHeader;
    BITMAPINFOHEADER bmpInfoHeader;
    RGBQUAD *palrgb;    // not array, pointer

	FILE* fp; 
	char input[128], output[128];
	
	int i, j, size; 
	float srcX, srcY;
	int index; 
	float r,g,b,gray;
	int graysize; 
	int index2;
	
	unsigned char *grayimg, *inimg, *outimg, *rgbquad;
	
	/* usage a.out in.bmp out.bmp */
	strcpy(input, argv[1]); 
	strcpy(output, argv[2]);
	
	
	if((fp=fopen(input, "rb")) == NULL) { 
		fprintf(stderr, "Error : Failed to open file...₩n"); 
		return -1;
	}
	
	fread(&bmpFileHeader, sizeof(bmpFileHeader), 1, fp); 
    fread(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, fp);

    if(bmpInfoHeader.biBitCount == 8 && bmpInfoHeader.biClrUsed == 0) {
            bmpInfoHeader.biClrUsed = 256;
    }
	
    printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
    printf("Bit Count : %d\n",bmpInfoHeader.biBitCount);

	size=widthbytes(bmpInfoHeader.biBitCount * bmpInfoHeader.biWidth); 
	graysize = widthbytes(8 * bmpInfoHeader.biWidth);
	
    unsigned int imagesize;
	if(!imagesize) 
		imagesize=bmpInfoHeader.biHeight*size;
	
    printf("Image Size : %d\n",imagesize);
    printf("Used Color : %d\n",bmpInfoHeader.biClrUsed);
	inimg=(BYTE*)malloc(sizeof(BYTE)*imagesize); 
	outimg=(BYTE*)malloc(sizeof(BYTE)*imagesize/3); 
    palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD)*256);
    //fread(palrgb, sizeof(RGBQUAD), bmpInfoHeader.biClrUsed, fp);

	fread(inimg, sizeof(BYTE), imagesize, fp); 

    for(int i = 0; i < bmpInfoHeader.biClrUsed; i++) {
            printf("Index %d : %u %u %u %u\n",i, palrgb[i].rgbRed, palrgb[i].rgbGreen, palrgb[i].rgbBlue, palrgb[i].rgbReserved);
    }

	fclose(fp);

    for (int i = 0; i < 256 ; i++) {
            palrgb[i].rgbRed = i;
            palrgb[i].rgbGreen = i;
            palrgb[i].rgbBlue = i;
    }
	
    float elemsize = bmpInfoHeader.biBitCount / 8.;
    int pos = 0;
    for(i = 0; i < bmpInfoHeader.biWidth * bmpInfoHeader.biHeight; i++) {
            r = inimg[(i*3)+2];
            g = inimg[(i*3)+1];
            b = inimg[(i*3)];
            gray = (r*0.3F)+(g*0.59F)+(b*0.11F);

            outimg[i] = gray;
    }

    bmpInfoHeader.biBitCount = 8;
    bmpInfoHeader.SizeImage = bmpInfoHeader.biWidth * bmpInfoHeader.biHeight;
    bmpInfoHeader.biClrUsed = 256;
    bmpFileHeader.bf0ffBits = sizeof(bmpInfoHeader) + sizeof(bmpFileHeader) + sizeof(RGBQUAD)*256;
	
	if((fp=fopen(output, "wb"))==NULL) { 
		fprintf(stderr, "Error : Failed to open file...₩n"); 
		return -1;
	}
	
	fwrite(&bmpFileHeader, sizeof(bmpFileHeader), 1, fp); 
	fwrite(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, fp); 
    fwrite(palrgb, sizeof(palrgb), 256, fp);
	fwrite(outimg, sizeof(unsigned char), bmpInfoHeader.SizeImage, fp);
	
	free(inimg); 
	free(outimg);
	
	fclose(fp); 
	
	return 0;
}
