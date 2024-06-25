#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

int main()
{
	/*
	 * mmap(
	 * where to put memory - NULL to let os decide,
	 * how much to allocate - multiple of pagesize desired,
	 * protection flags,
	 * other flags,
	 * file i/o,
	 * file i/o,
	 * )
	 */

	void *mp = mmap(NULL, 2, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	uint16_t *new_array = mp;	
	int num = 8191;
	new_array[num] = 15;

	printf("a1: %d\n", new_array[num]);
	printf("p: %p", mp);
	printf("test: %lu", sizeof(mp));
	return 0;
}
