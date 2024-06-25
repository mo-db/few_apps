#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
	int fd = open("file.txt", O_RDWR | O_CREAT, 0600);

	write(fd, "hello!", 6);

	sleep(30);

	return 0;
}
