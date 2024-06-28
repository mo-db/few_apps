#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define STANDARD_BUF_SIZE 5 
#define ITERATIONS 10000 


// generate random float between 0 & 1,
double get_prob();

// simulate dynamic need of bigger buffer
int expand_buf(int **, int);

// generate a random integer between 0 & 9, count++, 
// if buffer full return buffer, allocate new buffer,
// else return NULL
int gen_data(int **, int);

// if pointer NULL -> do nothing, 
// take sum of buffer and ad it to var=total
// free buffer
int process_data(int *, int);


int main()
{ 
	srand(time(NULL));
	//int new[buf_size];
	//int *fill_buf = new;
	int buf_size = STANDARD_BUF_SIZE;
	int *fill_buf = NULL;
	int buf_expand_flag = 0;
	int total = 0;

	for(int i = 0; i < ITERATIONS; i++) {
		if (buf_expand_flag) {
			buf_size = expand_buf(&fill_buf, buf_size);
			buf_expand_flag = 0;
		}

		int buffer_full = 0;
		if(get_prob() <= 0.40 ) {
			buffer_full = gen_data(&fill_buf, buf_size);
		}
		if(get_prob() <= 0.60) {
			if (buffer_full){
				int sum = process_data(fill_buf, buf_size);

				printf("adding %d to toal\n", sum);
				total += sum;
				
				printf("\nfree!\n\n");
				free(fill_buf);
				fill_buf = NULL;
				buf_size = STANDARD_BUF_SIZE;
			}
		} 
		if (get_prob() <= 0.05) {
			buf_expand_flag = 1;
		}
	}

	printf("Total: %d", total);
	return 0;
}


double get_prob()
{
	return (double)rand() / (double)RAND_MAX;
}


int gen_data(int **fill_buf_p, int buf_size)
{
	static int fill_count;
	printf("gen_data\n");
	if (*fill_buf_p == NULL) {
		printf("allocating memory for buffer!\n");
		*fill_buf_p = malloc(buf_size * sizeof(*fill_buf_p));
	}

	int random_value = (int)(rand() % 10);
	if (fill_count < (buf_size-1)) {
		(*fill_buf_p)[fill_count++] = random_value;
		return 0;
	} else {
		printf("buffer full! -> fill_count: %d\n", fill_count);
		fill_count = 0;
		return 1;
	}
}


int process_data(int *buf, int buf_size)
{
	printf("process_data\n");
	if (buf == NULL) {
		return 0;
	} else {
		int sum = 0;
		for (int i = 0; i < (buf_size-1); i++) {
			sum += buf[i++];	
		}
		return sum;
	}
}


int expand_buf(int **buf_p, int buf_size)
{
	int rand_num = (int)(rand() % 5);

	printf("expanding buffer by %d sizes\n", rand_num);
	int new_buf_size = buf_size + rand_num;
	printf("new buffer size: %d\n", new_buf_size);
	*buf_p = realloc(*buf_p, (new_buf_size * sizeof(*buf_p)));

	return new_buf_size;
}
