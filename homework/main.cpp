#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmpHeader.h"

typedef unsigned char UBYTE;         //unsigned char형 UBYTE로 선언

int main(int argc, char** argv)

{
    FILE* fp;                                       //파일 포인터 선언
    UBYTE* inimg, * outimg;                         //입출력 파일을 unsigned char형으로 선언
    BITMAPFILEHEADER bmpFileHeader;                 //bmpHeader의 FILEHEADER 선언
    BITMAPINFOHEADER bmpInfoHeader;                 //bmpHedaer의 INFOHEADER 선언
    RGBQUAD* palrgb;

    if (argc != 3)                                  //명령인자의 개수가 3개가 아닐 경우 오류 출력하고 프로그램 종료
    {
        fprintf(stderr, "Usgae : Fail to file open\n");
        return -1;
    }


    if ((fp = fopen(argv[1], "rb")) == NULL)        //argv[1] 자리에 입력된 input 파일을 "rb : 바이너리파일 읽기전용"으로 열기
    {
        fprintf(stderr, "Usage : Fail to file open\n");
        return -1;                                  
    }

    fread(&bmpFileHeader, sizeof(bmpFileHeader), 1, fp);                         //FILEHEADER 정보를 읽음
    fread(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, fp);                         //INFOHEADER 정보를 읽음

    int elemsize = bmpInfoHeader.biBitCount / 8;                                 //input 파일(image)이 24비트인 경우 한 픽셀당 r,g,b 3가지 요소가 포함됨을 표현
    int size = bmpInfoHeader.biWidth * elemsize;                                 //image의 가로 사이즈 (r,g,b 개수 포함)
    int imagesize = bmpInfoHeader.biWidth * bmpInfoHeader.biHeight * elemsize;   //image의 가로 * 세로 사이즈 (r,g,b 개수 포함)

    inimg = (UBYTE*)malloc(sizeof(UBYTE) * imagesize);                           //input image를 사용하기 위한 메모리 할당
    outimg = (UBYTE*)malloc(sizeof(UBYTE) * imagesize);                          //output image를 출력하기 위한 메모리 할당
    palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD) * 256);                            //팔렛트 사용을 위한 메모리 할당

    fread(palrgb, sizeof(RGBQUAD) * 256, 1, fp);                                 //팔렛트 256개의 정보를 읽음
    fread(inimg, sizeof(UBYTE), imagesize, fp);                                  //input image를 imageSize 만큼 읽음

    fclose(fp);                                                                  //사용한 파일 포인터를 닫는다

    for (int i = 0; i < 256; i++) {                                              //비어있는 팔렛트 채우기
        palrgb[i].rgbBlue = i;
        palrgb[i].rgbGreen = i;
        palrgb[i].rgbRed = i;
    } 

    /* imageSize만큼 돌면서 inimg의 픽셀당 b,g,r 값을 outimg의 픽셀당 b,g,r 값에 넣어줌 */
    for (int i = 0; i < imagesize; i += elemsize)
    {
        //outimg[i+0] = inimg[i+0];
        //outimg[i+1] = inimg[i+1];
        //outimg[i+2] = inimg[i+2];
        *(outimg + (i + 0)) = *(inimg + (i + 0));
        *(outimg + (i + 1)) = *(inimg + (i + 1));
        *(outimg + (i + 2)) = *(inimg + (i + 2));
    }



    if ((fp = fopen(argv[2], "wb")) == NULL)                                     //argv[2] 자리에 입력된 output 파일을 "wb : 바이너리파일 쓰기전용"으로 열기
    {
        fprintf(stderr, "File open fail\n");
        return -1;
    }

    fread(outimg, sizeof(UBYTE), imagesize, fp);                                 //output image를 imageSize 만큼 읽음

    fwrite(&bmpFileHeader, sizeof(bmpFileHeader), 1, fp);                        //FILEHEADER 정보를 읽음
    fwrite(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, fp);                        //INFOHEADER 정보를 읽음
    fwrite(palrgb, sizeof(RGBQUAD) * 256, 1, fp);


    bmpFileHeader.bfSize = sizeof(bmpFileHeader) + sizeof(bmpInfoHeader) + sizeof(RGBQUAD) * 256 + imagesize;   // 전체 사이즈를 FILEHEADER, INFOHEADER, 팔렛트의 크기만큼 지정
    bmpFileHeader.bfOffBits = sizeof(bmpFileHeader) + sizeof(bmpInfoHeader) + sizeof(RGBQUAD) * 256;            // 이미지 데이터를 넣기위해 Offset 이동
    fwrite(outimg, sizeof(UBYTE), imagesize, fp);                                                               // output image를 imagesize 만큼 입력                                      
    free(inimg);                                                                                                // 할당된 메모리 해제
    free(outimg);                                                                                               // 할당된 메모리 해제
    free(palrgb);                                                                                               // 할당된 메모리 해제
    fclose(fp);                                                                                                 // 사용한 파일 포인터 닫기

    return 0;
}