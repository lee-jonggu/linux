#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmpHeader.h"

#define BYTE    unsigned char
#define widthbytes(bits)    (((bits)+31) / 32*4)

int main(int argc, char** argv) {
        BITMAPFILEHEADER bmpFileHeader;
        BITMAPINFOHEADER bmpInfoHeader;

        FILE* fp;

        int i, j, size;

        unsigned char *inimg;

        unsigned int imagesize;

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
        fread(inimg, sizeof(BYTE)*imagesize, 1, fp); 

        fclose(fp);

        int elemsize = bmpInfoHeader.biBitCount / 8;
        int width = bmpInfoHeader.biWidth * elemsize;
        int R,G,B;

        for(j = 0; j < bmpInfoHeader.biHeight ; j++) {
                for(i = 0; i < bmpInfoHeader.biWidth * elemsize ; i += elemsize) {
                        B = inimg[i +j* bmpInfoHeader.biWidth * elemsize+0];
                        G = inimg[i +j* bmpInfoHeader.biWidth * elemsize+1];
                        R = inimg[i +j* bmpInfoHeader.biWidth * elemsize+2]; 
                        
                        printf("[B,G,R] : %d %d %d\n",inimg[i + j * width+0],
                                            inimg[i + j * width+1],
                                            inimg[i + j * width+2]);
                }
        }

        free(inimg);

        return 0;
}
