#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/fb.h>

#define  FB                      "/dev/fb0"
#define  NO_OF_COLOR   3

typedef unsigned char ubyte;

unsigned short makepixel(ubyte r, ubyte g, ubyte b) {
	return (unsigned short)( ((r>>3)<<11) | ((g>>2)<<5) | (b>>3) );
}

int main(int argc, char** argv) 
{
    int status, offset, i, j;
    unsigned char color[NO_OF_COLOR];
    unsigned short *pfb,pixel;

    struct fb_var_screeninfo fbvar;
    struct fb_fix_screeninfo fbfix;

    int fd = open(FB, O_RDWR);
    if(fd < 0) {
         printf("Error fbdev open\n"); 
         return -1;
    }

    status = ioctl(fd, FBIOGET_FSCREENINFO, &fbfix);
    status = ioctl(fd, FBIOPUT_VSCREENINFO, &fbvar);

    printf("x-resolution : %d\n", fbvar.xres);
    printf("y-resolution : %d\n", fbvar.yres);
    printf("x-resolution(virtual) : %d\n", fbvar.xres_virtual);	
    printf("y-resolution(virtual) : %d\n", fbvar.yres_virtual);
    printf("bpp : %d\n", fbvar.bits_per_pixel);
    printf("length of frame buffer memory : %d\n", fbfix.smem_len);

    fbvar.bits_per_pixel = 16;
    ioctl(fd, FBIOPUT_VSCREENINFO, &fbvar);

    
//    for(j=0;j<fbvar.yres;j++)
//            for(i=0;i<fbvar.xres;i++) {
//                    int offset = (j * fbvar.xres + i) * NO_OF_COLOR;
//                    lseek(fd, offset, SEEK_SET);
//                    color[0] = 255; color[1] = 255; color[2] = 255;
//                    write(fd,color,NO_OF_COLOR);
//            }
    close(fd);
    return 0;
}

