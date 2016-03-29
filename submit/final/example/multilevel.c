#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

const int M = 20, CNT = 20000;

//extern FILE *fout;

pthread_mutex_t level[20 + 1];

void *func_a() {
	int i, count = CNT;
	while (count--) {
		printf("a start %d\n", count);
		for (i = 0; i < M; i++) {
			printf("a want %d\n", i);
			pthread_mutex_lock(&level[i]);
		}
		puts("a");
		for (i = M - 1; i >= 0; i--) {
			pthread_mutex_unlock(&level[i]);
		}
		printf("a over %d\n", count);
	}
}

void *func_b() {
	int i, count = CNT;
	while (count--) {
		printf("b start %d\n", count);
		for (i = M - 1; i >= 0; i--) {
			printf("b want %d\n", i);
			pthread_mutex_lock(&level[i]);
		}
		puts("b");
		for (i = 0; i < M; i++) {
			pthread_mutex_unlock(&level[i]);
		}
		printf("b over %d\n", count);
	}
}

int main() {
	int i;
	for (i = 0; i < M; i++) {
		pthread_mutex_init(&level[i], 0);
	}
	pthread_t a, b;
	pthread_create(&a, NULL, func_a, NULL);
	pthread_create(&b, NULL, func_b, NULL);
	
	pthread_join (a, NULL);
	pthread_join (b, NULL);
	//fclose(fout);
	return 0;
}
