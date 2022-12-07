#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

#define FBDEVICE    "/dev/fb1"

extern inline unsigned short makepixel(unsigned char r, unsigned char g, unsigned char b)
{
        return (unsigned short)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
                        }

int main(void)
{
        struct fb_fix_screeninfo finfo;
        struct fb_var_screeninfo vinfo;
        unsigned short *pfb;
        int fbfd, size;

        fbfd = open(FBDEVICE, O_RDWR);
        if (fbfd < 0) {
                perror("Error : cannot open framebuffer device");
                return EXIT_FAILURE;
        }

        ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo);
        if (strcmp(finfo.id, "RPi-Sense FB") != 0) {
                printf("%s\n", "Error : RPi-Sense FB not found");
                close(fbfd);
                return EXIT_FAILURE;
        }

        if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
                perror("Error reading fixed information");
                return EXIT_FAILURE;
        }

        size = vinfo.xres * vinfo.yres * sizeof(short);

        pfb = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd,0);
        if (pfb == MAP_FAILED) {
                close(fbfd);
                perror("Error mmapping the file");
                return EXIT_FAILURE;
        }

        memset(pfb, 0, size);
//        *(pfb+0+0*vinfo.xres) = makepixel(255, 0, 0);
//        *(pfb+2+2*vinfo.xres) = makepixel(0, 255, 0);
//        *(pfb+4+4*vinfo.xres) = makepixel(0, 0, 255);
//        *(pfb+6+6*vinfo.xres) = makepixel(255, 0, 255);
        unsigned short r = makepixel(255,0,0);
        unsigned short o = makepixel(255,128,0);
        unsigned short y = makepixel(255,255,0);
        unsigned short g = makepixel(0,255,0);
        unsigned short b = makepixel(0,0,255);
        unsigned short n = makepixel(25,25,112);
        unsigned short p = makepixel(100,0,255);
        unsigned short number0[64] = {0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      r,r,r,r,r,r,r,r};

        unsigned short number1[64] = {0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      o,o,o,o,o,o,o,o,\
                                      0,0,0,0,0,0,0,0};

        unsigned short number2[64] = {0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      y,y,y,y,y,y,y,y,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0};

        unsigned short number3[64] = {0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      g,g,g,g,g,g,g,g,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0};

        unsigned short number4[64] = {0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      b,b,b,b,b,b,b,b,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0};

        unsigned short number5[64] = {0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      n,n,n,n,n,n,n,n,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0};

        unsigned short number6[64] = {0,0,0,0,0,0,0,0,\
                                      p,p,p,p,p,p,p,p,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0,\
                                      0,0,0,0,0,0,0,0};

        unsigned short number7[64] = {0,0,0,0,0,0,0,0,\
                                      p,p,p,p,p,p,p,p,\
                                      n,n,n,n,n,n,n,n,\
                                      b,b,b,b,b,b,b,b,\
                                      g,g,g,g,g,g,g,g,\
                                      y,y,y,y,y,y,y,y,\
                                      o,o,o,o,o,o,o,o,\
                                      r,r,r,r,r,r,r,r};
        for (int i = 0 ; i < 16 ; i++) {
        memcpy(pfb, number0,size);
        sleep(1);
        memcpy(pfb, number1,size);
        sleep(1);
        memcpy(pfb, number2,size);
        sleep(1);
        memcpy(pfb, number3,size);
        sleep(1);
        memcpy(pfb, number4,size);
        sleep(1);
        memcpy(pfb, number5,size);
        sleep(1);
        memcpy(pfb, number6,size);
        sleep(1);
        memcpy(pfb, number7,size);
        sleep(10);
        }

        if (munmap(pfb, size) == -1) {
                perror("Error un-mmapping the file");
        }

        close(fbfd);

        return EXIT_SUCCESS;
}
