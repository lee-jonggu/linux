#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmpHeader.h"

#define BYTE    unsigned char
#define widthbytes(bits)    (((bits)+31) / 32*4)

int main(int argc, char** argv) {
        BITMAPFILEHEADER bmpFileHeader;
        BITMAPINFOHEADER bmpInfoHeader;
        RGBQUAD *palrgb;

        FILE* fp;
        char input[128], output[128];

        int i, j, size;

        unsigned char *inimg, *outimg;

        unsigned int imagesize;

        strcpy(input,argv[1]);
        strcpy(output,argv[2]);

        if((fp=fopen(argv[1], "rb")) == NULL) {
                fprintf(stderr, "Error : Failed to open file... \n");
                return -1;
        }

        fread(&bmpFileHeader, sizeof(bmpFileHeader), 1, fp);
        fread(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, fp);

        size = widthbytes(bmpInfoHeader.biBitCount * bmpInfoHeader.biWidth);

        if(!imagesize)
                imagesize = size * bmpInfoHeader.biHeight;

        inimg = (BYTE*)malloc(sizeof(BYTE)*imagesize);
        outimg = (BYTE*)malloc(sizeof(BYTE)*imagesize);
        palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD)*256);
        fread(inimg, sizeof(BYTE)*imagesize, 1, fp); 

        fclose(fp);

        for(i = 0; i < 256; i++) {
            palrgb[i].rgbBlue = i;
            palrgb[i].rgbGreen = i;
            palrgb[i].rgbRed = i;
        }

        
        float elemsize = bmpInfoHeader.biBitCount / 8.;
        int width = bmpInfoHeader.biWidth * elemsize;
        int R,G,B;

        for(j = 0; j < bmpInfoHeader.biHeight ; j++) {
                for(i = 0; i < bmpInfoHeader.biWidth * elemsize ; i += elemsize) {
                        outimg[i + j * width + 2] = inimg[i + j * width + 2];
                        outimg[i + j * width + 1] = inimg[i + j * width + 1];
                        outimg[i + j * width + 0] = inimg[i + j * width + 0];
                }
        }


        if((fp=fopen(argv[2], "wb")) == NULL) {
                fprintf(stderr, "Error : Failed to open file... \n");
                return -1;
        }
        bmpFileHeader.bfSize = sizeof(bmpFileHeader) + sizeof(bmpInfoHeader) + sizeof(RGBQUAD)*256 + imagesize;
        bmpFileHeader.bf0ffBits = sizeof(bmpFileHeader) + sizeof(bmpInfoHeader) + sizeof(RGBQUAD)*256;


        fwrite(&bmpFileHeader, sizeof(bmpFileHeader), 1, fp);
	    fwrite(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, fp);
        fwrite(palrgb, sizeof(palrgb), 256, fp);

        fwrite(outimg, sizeof(BYTE)*imagesize, 1, fp); 

        free(inimg);
        free(outimg);

        return 0;
}
