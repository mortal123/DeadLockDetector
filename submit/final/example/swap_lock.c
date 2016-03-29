#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

//extern FILE *fout;

pthread_mutex_t first = PTHREAD_MUTEX_INITIALIZER, 
				second = PTHREAD_MUTEX_INITIALIZER;

void *func_a() {
	int count = 2000;
	while (count--) {
		pthread_mutex_lock(&first);
		pthread_mutex_lock(&second);
		puts("a");
		pthread_mutex_unlock(&second);
		pthread_mutex_unlock(&first);
		
	}
}

void *func_b() {
	int count = 2000;
	while (count--) {
		pthread_mutex_lock(&second);
		pthread_mutex_lock(&first);
		puts("b");
		pthread_mutex_unlock(&first);
		pthread_mutex_unlock(&second);
	}
}

int main() {
	printf("program begin\n");
	pthread_t a, b;
	pthread_create(&a, NULL, func_a, NULL);
	pthread_create(&b, NULL, func_b, NULL);
	
	pthread_join (a, NULL);
	pthread_join (b, NULL);
	//fclose(fout);
	return 0;
}
