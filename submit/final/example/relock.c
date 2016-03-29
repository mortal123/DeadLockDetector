#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

pthread_mutex_t first = PTHREAD_MUTEX_INITIALIZER;

int main() {
	pthread_mutex_lock(&first);
	pthread_mutex_lock(&first);
	pthread_mutex_unlock(&first);
	return 0;
}
