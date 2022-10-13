#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

int main()
{
   umask(0);
   char *fname = "test.txt";
   int  fd;
   if((fd = creat(fname, 0666)) < 0) {
         perror("creat( )");
         return -1;
   }
   printf("First printf is on the screen.\n");   
   dup2(fd,1); 
   printf("Second printf is in this file.\n");
   return 0;   
}

