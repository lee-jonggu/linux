#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
	int fd = open("test.txt", O_RDONLY);

	printf("%d\n", fd);
	close(fd);

	return 0;
}
