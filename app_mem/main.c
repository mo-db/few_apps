#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

typedef struct myiob {
	int count;
} MYFILE;

#define MAX_SIZE 20
MYFILE my_ioary[MAX_SIZE];

#define mystdin &my_ioary[0]

int main(int argc, char **argv)
{
	return 0;
}
