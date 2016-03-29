#include <stdio.h>
#include <pthread.h>
#define MAX 10000 //需要生产的数量 
pthread_mutex_t the_mutex;
pthread_cond_t condc, condp;
int buffer = 0; //生产者消费者使用的缓冲区 

void *producer(void *ptr) //生产数据
{
	int i;
	for (i = 1; i <= MAX; i++)
	{
		pthread_mutex_lock(&the_mutex); //互斥使用缓冲区
		if (buffer != 0)
			pthread_cond_wait(&condp, &the_mutex);
		buffer = i; //将数据放入缓冲区 
		pthread_cond_signal(&condc); //唤醒消费者
		pthread_mutex_unlock(&the_mutex); //释放缓冲区
	}
	pthread_exit(0);
}

void *consumer(void *ptr) //消费数据 
{
	int i;
	for (i = 1; i <= MAX; i++)
	{
		pthread_mutex_lock(&the_mutex); //互斥使用缓冲区
		if (buffer == 0)
			pthread_cond_wait(&condc, &the_mutex);
		buffer = 0; //从缓冲区中取出数据 
		pthread_cond_signal(&condp); //唤醒生产者 
		pthread_mutex_unlock(&the_mutex); //释放缓冲区
	}
	pthread_exit(0);
}

int main(int argc, char **argv) 
{
	pthread_t pro, con;
	pthread_mutex_init(&the_mutex, 0);
	pthread_cond_init(&condc, 0);
	pthread_cond_init(&condp, 0);
	pthread_create(&con, 0, consumer, 0);
	pthread_create(&pro, 0, producer, 0);
	pthread_join(pro, 0);
	pthread_join(con, 0);
	//pthread_cond_destroy(&condc);
	//pthread_cond_destroy(&condp);
	//pthread_mutex_destory(&the_mutex);
	return 0;
}
