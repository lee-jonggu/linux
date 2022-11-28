/* 
* Framebuffer & V4L2 video capture example   
* 
* This program can be used and distributed without restrictions. 
*/ 
 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <assert.h> 
#include <getopt.h>  
#include <fcntl.h>  
#include <unistd.h> 
#include <errno.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <sys/time.h> 
#include <sys/mman.h> 
#include <sys/ioctl.h> 
#include <asm/types.h> /* for videodev2.h */ 
#include <linux/videodev2.h> 
#include <linux/fb.h> 

#define CLEAR(x) memset(&(x), 0, sizeof(x)) 
#define WIDTH	640
#define HEIGHT	480
 
struct buffer { 
    void * start; 
    size_t length; 
}; 
 
static char * dev_name = NULL; 
static int fd = -1; 
struct buffer * buffers = NULL; 
static unsigned int n_buffers = 0; 
static int time_in_sec_capture=5; 
static int fbfd = -1; 
static struct fb_var_screeninfo vinfo; 
static struct fb_fix_screeninfo finfo; 
static short *fbp=NULL; 
static long screensize=0; 
 
static void errno_exit(const char * s) 
{ 
    fprintf(stderr, "%s error %d, %s\n",s, errno, strerror(errno)); 
    exit(EXIT_FAILURE); 
} 
 
static int xioctl(int fd, int request, void * arg) 
{ 
    int r; 
    do r = ioctl(fd, request, arg); 
    while(r == -1 && errno == EINTR); 
    return r; 
} 
 
inline int clip(int value, int min, int max) 
{ 
    return(value > max ? max : value < min ? min : value); 
} 
 
static void process_image(const void *p)
{
    //ConvertYUYVToRGB16 
    unsigned char* in = (char*)p; 
    int width = WIDTH; 
    int height = HEIGHT; 

    unsigned short pixel;
    int istride = WIDTH*2;	//1280; 
    int x, y, j; 
    int y0, u, y1, v, r, g, b; 
    long location = 0; 
    unsigned short output[WIDTH*HEIGHT*2];
    for(y = 0; y < height /*+ 100*/; ++y) { 
        for(j = 0, x = 0; j < width * 2 ; j += 4, x += 2) {
            
            y0 = in[j]; 
            u = in[j + 1] - 128;                 
            y1 = in[j + 2];         
            v = in[j + 3] - 128;         
 
            r =(298 * y0 + 409 * v + 128) >> 8; 
            g =(298 * y0 - 100 * u - 208 * v + 128) >> 8; 
            b =(298 * y0 + 516 * u + 128) >> 8; 
	    r = clip(r, 0, 255); 
	    g = clip(g, 0, 255); 
	    b = clip(b, 0, 255); 
            pixel = ((r>>3)<<11)|((g>>2)<<5)|(b>>3);
            fbp[location++] = pixel;     

            r =(298 * y1 + 409 * v + 128) >> 8; 
            g =(298 * y1 - 100 * u - 208 * v + 128) >> 8; 
            b =(298 * y1 + 516 * u + 128) >> 8;
	    r = clip(r, 0, 255); 
	    g = clip(g, 0, 255); 
	    b = clip(b, 0, 255); 
            pixel = ((r>>3)<<11)|((g>>2)<<5)|(b>>3);
            fbp[location++] = pixel;     

        } 
        in +=istride; 
    }
} 
 
static int read_frame(void) 
{ 
    struct v4l2_buffer buf; 
    unsigned int i; 
 
    CLEAR(buf); 
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
    buf.memory = V4L2_MEMORY_MMAP; 
 
    if(xioctl(fd, VIDIOC_DQBUF, &buf) == -1) { 
        switch(errno) { 
        case EAGAIN: 
            return 0; 
        case EIO:    /* Could ignore EIO, see spec. */ 
        /* fall through */ 
            perror("EIO");
        default: 
            errno_exit("VIDIOC_DQBUF"); 
        }; 
    } 
 
    assert(buf.index < n_buffers);
    assert(buf.field ==V4L2_FIELD_NONE); 
    process_image(buffers[buf.index].start); 
    if(xioctl(fd, VIDIOC_QBUF, &buf) == -1) 
        errno_exit("VIDIOC_QBUF"); 
 
    return 1; 
} 
 
static void run(void) 
{ 
    unsigned int count; 
    int frames; 
    frames = 30 * time_in_sec_capture; 
 
    while(frames-- > 0) { 
        for(;;) { 
            fd_set fds; 
            struct timeval tv; 
            int r; 
            FD_ZERO(&fds); 
            FD_SET(fd, &fds); 
 
            /* Timeout. */ 
            tv.tv_sec = 2; 
            tv.tv_usec = 0; 
 
            r = select(fd + 1, &fds, NULL, NULL, &tv); 
	    switch(r) {
		case -1: 
                    if(EINTR == errno) 
                        continue; 
                    errno_exit("select"); 
                    break;
                case 0:
                    fprintf(stderr, "select timeout\n"); 
                    exit(EXIT_FAILURE); 
                    break;
            };
 
            if(read_frame()) 
                break; 
            /* EAGAIN - continue select loop. */ 
        }; 
    };
} 
 
static void stop_capturing(void) 
{ 
    enum v4l2_buf_type type; 
 
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
    if(xioctl(fd, VIDIOC_STREAMOFF, &type) == -1) 
        errno_exit("VIDIOC_STREAMOFF"); 
} 
 
static void start_capturing(void) 
{ 
    unsigned int i; 
    enum v4l2_buf_type type; 
 
    for(i = 0; i < n_buffers; ++i) { 
        struct v4l2_buffer buf; 
        CLEAR(buf); 
 
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
        buf.memory = V4L2_MEMORY_MMAP; 
        buf.index = i; 
 
        if(xioctl(fd, VIDIOC_QBUF, &buf) == -1)
            errno_exit("VIDIOC_QBUF"); 
    };
 
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
 
    if(xioctl(fd, VIDIOC_STREAMON, &type) == -1) 
        errno_exit("VIDIOC_STREAMON"); 
} 
 
static void uninit_device(void) 
{ 
    unsigned int i; 
 
    for(i = 0; i < n_buffers; ++i) 
        if(munmap(buffers[i].start, buffers[i].length) == -1) 
            errno_exit("munmap"); 
     
    if(munmap(fbp, screensize) == -1) { 
          printf(" Error: framebuffer device munmap() failed.\n"); 
          exit(EXIT_FAILURE) ; 
    };

    free(buffers); 
} 
 
 
static void init_mmap(void) 
{ 
    struct v4l2_requestbuffers req; 

    //mmap framebuffer 
    fbp = (short *)mmap(NULL, screensize, PROT_READ | PROT_WRITE,MAP_SHARED, fbfd, 0); 
    if((int)fbp == -1) { 
        printf("Error: failed to map framebuffer device to memory.\n"); 
        exit(EXIT_FAILURE) ; 
    } 
    memset(fbp, 0, screensize); 

    CLEAR(req); 
    req.count = 4; 
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
    req.memory = V4L2_MEMORY_MMAP; 
    if(xioctl(fd, VIDIOC_REQBUFS, &req) == -1) { 
        if(errno == EINVAL) { 
            fprintf(stderr, "%s does not support memory mapping\n", dev_name); 
            exit(EXIT_FAILURE); 
        } else { 
            errno_exit("VIDIOC_REQBUFS"); 
        } 
    } 
 
    if(req.count < 4) {    //if(req.count < 2) 
        fprintf(stderr, "Insufficient buffer memory on %s\n",dev_name); 
        exit(EXIT_FAILURE); 
    } 
 
    buffers = calloc(req.count, sizeof(*buffers)); 
    if(!buffers) { 
        fprintf(stderr, "Out of memory\n"); 
        exit(EXIT_FAILURE); 
    } 
 
    for(n_buffers = 0; n_buffers < req.count; ++n_buffers) { 
        struct v4l2_buffer buf; 
 
        CLEAR(buf); 
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
        buf.memory = V4L2_MEMORY_MMAP; 
        buf.index = n_buffers; 
 
        if(xioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) 
            errno_exit("VIDIOC_QUERYBUF"); 
 
        buffers[n_buffers].length = buf.length; 
        buffers[n_buffers].start =mmap(NULL,buf.length,PROT_READ | PROT_WRITE ,MAP_SHARED,fd, buf.m.offset); 
 
        if(MAP_FAILED == buffers[n_buffers].start) 
            errno_exit("mmap"); 
    }
} 
 
static void init_device(void) 
{ 
    struct v4l2_capability cap; 
    struct v4l2_cropcap cropcap; 
    struct v4l2_crop crop; 
    struct v4l2_format fmt; 
    unsigned int min; 
 
    // Get fixed screen information 
    if(xioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) { 
        printf("Error reading fixed information.\n"); 
        exit(EXIT_FAILURE); 
    } 
 
    // Get variable screen information 
    if(xioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) { 
         printf("Error reading variable information.\n"); 
         exit(EXIT_FAILURE); 
    } 

    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8; 
 
    if(xioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) { 
        if(errno == EINVAL) { 
            fprintf(stderr, "%s is no V4L2 device\n",dev_name); 
            exit(EXIT_FAILURE); 
        } else { 
            errno_exit("VIDIOC_QUERYCAP"); 
        } 
    } 
 
    if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) { 
        fprintf(stderr, "%s is no video capture device\n",dev_name); 
        exit(EXIT_FAILURE); 
    } 
 
    if(!(cap.capabilities & V4L2_CAP_STREAMING)) { 
        fprintf(stderr, "%s does not support streaming i/o\n",dev_name); 
        exit(EXIT_FAILURE); 
    } 
 
    /* Select video input, video standard and tune here. */ 
    CLEAR(cropcap); 
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
    if(xioctl(fd, VIDIOC_CROPCAP, &cropcap) == 0) { 
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
        crop.c = cropcap.defrect; /* reset to default */ 
 
        if(xioctl(fd, VIDIOC_S_CROP, &crop) == -1) { 
            switch(errno) { 
            case EINVAL:    /* Cropping not supported. */ 
                break; 
            default:/* Errors ignored. */ 
                 break; 
            } 
        } 
    } else { /* Errors ignored. */    } 
 
    CLEAR(fmt); 
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
    fmt.fmt.pix.width = WIDTH;  
    fmt.fmt.pix.height = HEIGHT; 
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; 
    fmt.fmt.pix.field = V4L2_FIELD_NONE; 
 
    if(xioctl(fd, VIDIOC_S_FMT, &fmt) == -1) 
        errno_exit("VIDIOC_S_FMT"); 
 
    /* Note VIDIOC_S_FMT may change width and height. */ 
 
    /* Buggy driver paranoia. */ 
    /*
    min = fmt.fmt.pix.width * 2; 
    if(fmt.fmt.pix.bytesperline < min) 
        fmt.fmt.pix.bytesperline = min; 
    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height; 
    if(fmt.fmt.pix.sizeimage < min) 
        fmt.fmt.pix.sizeimage = min; 
    */ 
    init_mmap(); 
 
} 
 
static void close_device(void) 
{ 
    if(close(fd) == -1) 
        errno_exit("close"); 
    fd = -1; 
    close(fbfd); 
} 
 
static void open_device(void) 
{ 
    struct stat st;  
 
    if(stat(dev_name, &st) == -1) { 
        fprintf(stderr, "Cannot identify '%s': %d, %s\n", dev_name, errno, strerror(errno)); 
        exit(EXIT_FAILURE); 
    } 
 
    if(!S_ISCHR(st.st_mode)) { 
        fprintf(stderr, "%s is no device\n", dev_name); 
        exit(EXIT_FAILURE); 
    } 
 
    //open framebuffer 
    fbfd = open("/dev/fb0", O_RDWR); 
    if(fbfd == -1) { 
        printf("Error: cannot open framebuffer device.\n"); 
        exit(EXIT_FAILURE); 
    }
 
    //open camera 
    fd = open(dev_name, O_RDWR| O_NONBLOCK, 0); 
    if(fd == -1) { 
         fprintf(stderr, "Cannot open '%s': %d, %s\n",dev_name, errno, strerror(errno)); 
         exit(EXIT_FAILURE); 
    } 
} 
 
static void usage(FILE * fp,int argc,char ** argv) 
{ 
     fprintf(fp, 
          "Usage: %s [options]\n\n" 
          "Options:\n" 
          "-d | --device name Video device name [/dev/video]\n" 
          "-h | --help Print this message\n"
          "-t | --how long will display in seconds\n" 
          "", 
          argv[0]); 
} 
 
static const char short_options [] = "d:ht:"; 
static const struct option long_options [] = { 
     { "device", required_argument, NULL, 'd' }, 
     { "help", no_argument, NULL, 'h' },
     { "time", no_argument, NULL, 't' }, 
     { 0, 0, 0, 0 } 
}; 
 
int main(int argc,char ** argv) 
{ 
    dev_name = "/dev/video0"; 
 
    for(;;) { 
        int index; 
        int c; 
 
        c = getopt_long(argc, argv,short_options, long_options,&index); 
        if(-1 == c) 
            break; 
 
        switch(c) { 
        case 0: /* getopt_long() flag */ 
            break; 
        case 'd': 
            dev_name = optarg; 
            break; 
        case 'h': 
            usage(stdout, argc, argv); 
            exit(EXIT_SUCCESS); 
        case 't':
             time_in_sec_capture = atoi(optarg);
             break;
        default: 
            usage(stderr, argc, argv); 
            exit(EXIT_FAILURE); 
        }; 
    };

    open_device(); 
 
    init_device(); 
 
    start_capturing(); 
 
    run(); 
 
    stop_capturing(); 
 
    uninit_device(); 
 
    close_device(); 
 
    exit(EXIT_SUCCESS); 
 
    return 0; 
} 


