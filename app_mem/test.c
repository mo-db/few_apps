#include <stdio.h>

void test_func(int **test_pointer)
{
	*test_pointer = NULL;
}

int main()
{
	int * test_p1 = NULL;
	int test_value = 5;
	test_p1 = &test_value;

	test_func(&test_p1);
	if (test_p1) {
		printf("testp1: %d", *test_p1);
	}
	return 0;
}
