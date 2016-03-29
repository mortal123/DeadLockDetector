#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

const int M = 20, CNT = 1;

//extern FILE *fout;

pthread_mutex_t spoon[2000 + 1];

void *eat(int i) {
	int count = 5;
	while (count--) {
		pthread_mutex_lock(&spoon[(i + M - 1) % M]);
		sleep(5);
		pthread_mutex_lock(&spoon[(i + 1) % M]);
		
		
		pthread_mutex_lock(&spoon[(i + 1) % M]);
		pthread_mutex_lock(&spoon[(i + M - 1) % M]);
	}
}

int main() {
	int i, j;
	for (i = 0; i < 2000; i++) {
		pthread_mutex_init(&spoon[i], 0);
	}
	pthread_t man[2000];
	for (i = 0; i < CNT; i++) {
		for (j = 0; j < M; j++) {
			pthread_create(&man[j], NULL, eat(j), NULL);
		}
		for (j = 0; j < M; j++) {
			pthread_join(man[j], 0);
		}
	}
	
	//fclose(fout);
	return 0;
}
