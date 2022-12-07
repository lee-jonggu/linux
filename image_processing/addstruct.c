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
	FILE* fp; 
	RGBQUAD palrgb[256];
    BITMAPFILEHEADER bmpFileHeader;
    BITMAPINFOHEADER bmpInfoHeader;
	
	char input[128], output[128];
	
	int i, j, size; 
	float srcX, srcY;
	int index; 
	float r,g,b,gray;
	int graysize; 
	int index2;
	
	unsigned char *grayimg, *inimg, *outimg;
	
	/* usage a.out in.bmp out.bmp */
	strcpy(input, argv[1]); 
	strcpy(output, argv[2]);
	
	
	if((fp=fopen(input, "rb")) == NULL) { 
		fprintf(stderr, "Error : Failed to open file...₩n"); 
		return -1;
	}
	
    fread(&bmpFileHeader, sizeof(bmpFileHeader),1,fp);
    fread(&bmpInfoHeader, sizeof(bmpInfoHeader),1,fp);
	
    unsigned int imagesize = bmpInfoHeader.SizeImage;
	size=widthbytes(bmpInfoHeader.biBitCount * bmpInfoHeader.biWidth); 
	graysize = widthbytes(8 * bmpInfoHeader.biWidth);
	if(!imagesize) 
		imagesize=bmpInfoHeader.biHeight*size;
	
	inimg=(BYTE*)malloc(sizeof(BYTE)*imagesize); 
	outimg=(BYTE*)malloc(sizeof(BYTE)*imagesize); 
	fread(inimg, sizeof(BYTE), imagesize, fp); 
	
	fclose(fp);
	
    printf("[%s] %s : %d\n",__FILE__,__FUNCTION__,__LINE__);
	for(i=0; i<bmpInfoHeader.biHeight*3; i+=3) { 
		for(j=0; j<bmpInfoHeader.biWidth*3; j+=3) {
			int b = inimg[j+(i*bmpInfoHeader.biWidth+0)]; // unsigned char 0 ~ 255
			int g = inimg[j+(i*bmpInfoHeader.biWidth+1)]; 
			int r = inimg[j+(i*bmpInfoHeader.biWidth+2)]; 
			
			b += 50;
			g += 50;
			r += 50; 
			
			outimg[j+bmpInfoHeader.biWidth*i+0]= clip(b, 0, 255);
			outimg[j+bmpInfoHeader.biWidth*i+1]= clip(g, 0, 255);
			outimg[j+bmpInfoHeader.biWidth*i+2]= clip(r, 0, 255);
		};
    //printf("[%s] %s : %d\n",__FILE__,__FUNCTION__,__LINE__);
	 };	   
	 
    printf("[%s] %s : %d\n",__FILE__,__FUNCTION__,__LINE__);
	size=widthbytes(bmpInfoHeader.biBitCount*bmpInfoHeader.biWidth); 
	bmpInfoHeader.SizeImage=bmpInfoHeader.biHeight*bmpInfoHeader.biSize; 
	bmpFileHeader.bf0ffBits+=256*sizeof(RGBQUAD);
	
    printf("[%s] %s : %d\n",__FILE__,__FUNCTION__,__LINE__);
	if((fp=fopen(output, "wb"))==NULL) { 
		fprintf(stderr, "Error : Failed to open file...₩n"); 
		return -1;
	}
	
    fwrite(&bmpFileHeader, sizeof(bmpFileHeader),1,fp);
    fwrite(&bmpInfoHeader, sizeof(bmpInfoHeader),1,fp);
	
	fwrite(outimg, sizeof(unsigned char), bmpInfoHeader.SizeImage, fp);
	
	free(inimg); 
	free(outimg);
	
	fclose(fp); 
	
	return 0;
}
