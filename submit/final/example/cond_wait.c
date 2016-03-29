#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_cond_t cond;
pthread_mutex_t s,b;
pthread_t aa,bb;

void *fa(void *argv)
{
    pthread_mutex_lock(&s);
    pthread_mutex_lock(&b);
    printf("Done\n");
    pthread_cond_wait(&cond, &s);
    printf("Start?\n");
    pthread_mutex_unlock(&s);
    pthread_mutex_unlock(&b);
    pthread_exit(0);
}
void *fb(void *argv)
{
    sleep(1);
    pthread_mutex_lock(&s);
    printf("Locked\n");
    pthread_cond_signal(&cond);
    printf("Sent\n");
    pthread_mutex_lock(&b);
    pthread_mutex_unlock(&s);
    pthread_mutex_unlock(&b);
    pthread_exit(0);
}
int main()
{
    pthread_mutex_init(&s, 0);
    pthread_mutex_init(&b, 0);
    pthread_cond_init(&cond, 0);
    pthread_create(&aa, 0, fa, 0);
    pthread_create(&bb, 0, fb, 0);
    pthread_join(aa,0);
    pthread_join(bb,0);
    pthread_mutex_destroy(&s);
    pthread_mutex_destroy(&b);
    pthread_cond_destroy(&cond);
    return 0;
}

