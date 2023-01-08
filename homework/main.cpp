#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmpHeader.h"

typedef unsigned char UBYTE;         //unsigned char�� UBYTE�� ����

int main(int argc, char** argv)

{
    FILE* fp;                                       //���� ������ ����
    UBYTE* inimg, * outimg;                         //����� ������ unsigned char������ ����
    BITMAPFILEHEADER bmpFileHeader;                 //bmpHeader�� FILEHEADER ����
    BITMAPINFOHEADER bmpInfoHeader;                 //bmpHedaer�� INFOHEADER ����
    RGBQUAD* palrgb;

    if (argc != 3)                                  //��������� ������ 3���� �ƴ� ��� ���� ����ϰ� ���α׷� ����
    {
        fprintf(stderr, "Usgae : Fail to file open\n");
        return -1;
    }


    if ((fp = fopen(argv[1], "rb")) == NULL)        //argv[1] �ڸ��� �Էµ� input ������ "rb : ���̳ʸ����� �б�����"���� ����
    {
        fprintf(stderr, "Usage : Fail to file open\n");
        return -1;                                  
    }

    fread(&bmpFileHeader, sizeof(bmpFileHeader), 1, fp);                         //FILEHEADER ������ ����
    fread(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, fp);                         //INFOHEADER ������ ����

    int elemsize = bmpInfoHeader.biBitCount / 8;                                 //input ����(image)�� 24��Ʈ�� ��� �� �ȼ��� r,g,b 3���� ��Ұ� ���Ե��� ǥ��
    int size = bmpInfoHeader.biWidth * elemsize;                                 //image�� ���� ������ (r,g,b ���� ����)
    int imagesize = bmpInfoHeader.biWidth * bmpInfoHeader.biHeight * elemsize;   //image�� ���� * ���� ������ (r,g,b ���� ����)

    inimg = (UBYTE*)malloc(sizeof(UBYTE) * imagesize);                           //input image�� ����ϱ� ���� �޸� �Ҵ�
    outimg = (UBYTE*)malloc(sizeof(UBYTE) * imagesize);                          //output image�� ����ϱ� ���� �޸� �Ҵ�
    palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD) * 256);                            //�ȷ�Ʈ ����� ���� �޸� �Ҵ�

    fread(palrgb, sizeof(RGBQUAD) * 256, 1, fp);                                 //�ȷ�Ʈ 256���� ������ ����
    fread(inimg, sizeof(UBYTE), imagesize, fp);                                  //input image�� imageSize ��ŭ ����

    fclose(fp);                                                                  //����� ���� �����͸� �ݴ´�

    for (int i = 0; i < 256; i++) {                                              //����ִ� �ȷ�Ʈ ä���
        palrgb[i].rgbBlue = i;
        palrgb[i].rgbGreen = i;
        palrgb[i].rgbRed = i;
    } 

    /* imageSize��ŭ ���鼭 inimg�� �ȼ��� b,g,r ���� outimg�� �ȼ��� b,g,r ���� �־��� */
    for (int i = 0; i < imagesize; i += elemsize)
    {
        //outimg[i+0] = inimg[i+0];
        //outimg[i+1] = inimg[i+1];
        //outimg[i+2] = inimg[i+2];
        *(outimg + (i + 0)) = *(inimg + (i + 0));
        *(outimg + (i + 1)) = *(inimg + (i + 1));
        *(outimg + (i + 2)) = *(inimg + (i + 2));
    }



    if ((fp = fopen(argv[2], "wb")) == NULL)                                     //argv[2] �ڸ��� �Էµ� output ������ "wb : ���̳ʸ����� ��������"���� ����
    {
        fprintf(stderr, "File open fail\n");
        return -1;
    }

    fread(outimg, sizeof(UBYTE), imagesize, fp);                                 //output image�� imageSize ��ŭ ����

    fwrite(&bmpFileHeader, sizeof(bmpFileHeader), 1, fp);                        //FILEHEADER ������ ����
    fwrite(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, fp);                        //INFOHEADER ������ ����
    fwrite(palrgb, sizeof(RGBQUAD) * 256, 1, fp);


    bmpFileHeader.bfSize = sizeof(bmpFileHeader) + sizeof(bmpInfoHeader) + sizeof(RGBQUAD) * 256 + imagesize;   // ��ü ����� FILEHEADER, INFOHEADER, �ȷ�Ʈ�� ũ�⸸ŭ ����
    bmpFileHeader.bfOffBits = sizeof(bmpFileHeader) + sizeof(bmpInfoHeader) + sizeof(RGBQUAD) * 256;            // �̹��� �����͸� �ֱ����� Offset �̵�
    fwrite(outimg, sizeof(UBYTE), imagesize, fp);                                                               // output image�� imagesize ��ŭ �Է�                                      
    free(inimg);                                                                                                // �Ҵ�� �޸� ����
    free(outimg);                                                                                               // �Ҵ�� �޸� ����
    free(palrgb);                                                                                               // �Ҵ�� �޸� ����
    fclose(fp);                                                                                                 // ����� ���� ������ �ݱ�

    return 0;
}