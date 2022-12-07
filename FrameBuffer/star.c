#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <math.h>

#define FBDEVICE "/dev/fb0"

typedef unsigned char ubyte;

unsigned short makepixel(unsigned char r, unsigned char g, unsigned char b) {
    return (unsigned short)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
}

static int DrawStart(int fd, int x, int y, unsigned short color)
{
    struct fb_var_screeninfo vinfo;
    unsigned short *pfb;
    double alpha = (2 * M_PI) / 10; 
    int offset, radius = 12; 

    /* 현재 프레임버퍼에 대한 고정된 화면 정보를 얻어온다. */
    if(ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        perror("Error reading fixed information");
        return -1;
    }
 
    pfb = (unsigned short *)mmap(0, vinfo.xres_virtual*vinfo.yres_virtual*2, PROT_READ | PROT_WRITE,
                                                     MAP_SHARED, fd, 0);
    for(int i = 11; i != 0; i--) {
        double r = radius*(i % 2 + 1)/2;
        double omega = alpha * i;
        int pos = (int)(r * sin(omega) + x 
              + (r * cos(omega) + y) * vinfo.xres_virtual);
        *(pfb + pos) = color;
    }

    munmap(pfb, vinfo.xres*vinfo.yres*2);

    return 0;
}

int main(int argc, char** argv)
{
    int fbfd, status, offset;
    /* 프레임버퍼 정보 처리를 위한 구조체 */
    unsigned short pixel;

    fbfd = open(FBDEVICE, O_RDWR);    /* 사용할 프레임버퍼 장치를 오픈한다. */
    if(fbfd < 0) {
        perror("Error: cannot open framebuffer device");
        return -1;
    }

    DrawStart(fbfd, 0, 0, makepixel(255, 0, 255)) ;         /* Magenta색상을 생성 */ 

    close(fbfd);                                           /* 사용이 끝난 프레임버
퍼 장치를 닫는다. */

    return 0;
}

