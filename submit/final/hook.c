#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <pthread.h>

const int debug = 0;
static int BOUND = 1000007;

FILE* fout; 

typedef long long UI;

void DEBUG(char *s) {
	if (!debug) {
		return ;
	}
	fputs(s, fout);
	fputs("\n", fout);
}

typedef int(*MUTEX_LOCK)(pthread_mutex_t *);
MUTEX_LOCK old_mutex_lock = NULL;

typedef int(*MUTEX_UNLOCK)(pthread_mutex_t *);
MUTEX_UNLOCK old_mutex_unlock = NULL;

typedef int(*COND_WAIT)(pthread_cond_t *, pthread_mutex_t *);
COND_WAIT old_cond_wait = NULL;

typedef int(*COND_SIGNAL)(pthread_cond_t *);
COND_SIGNAL old_cond_signal = NULL;

pthread_mutex_t hook_mutex = PTHREAD_MUTEX_INITIALIZER;

UI cpid[1000007];
pthread_mutex_t* cmid[1000007];

struct HashTable {
	int head[1000007], next[1000007], tot;
	UI value[1000007];
};

struct HashTable table;
int go[1000007], mutex_set[1000007];

int hash_code(UI x) {
	return (x % BOUND + BOUND) % BOUND;
}

void hash_clear(struct HashTable *table) {
	int i;
	for (i = 0; i < BOUND; i++) {
		table->head[i] = table->next[i] = -1;
		go[i] = mutex_set[i] = -1;
		cpid[i] = cmid[i] = -1;
	}
	table->tot = 0;
}

int table_find(struct HashTable *table, UI a) {
DEBUG("fuck");
	int ta = hash_code(a), ptr;
	for (ptr = table->head[ta]; ptr != -1; ptr = table->next[ptr]) {
		if (table->value[ptr] == a) {
			return ptr;
		}
	}
	
	int tot = table->tot;
	table->next[tot] = table->head[ta];
	table->head[ta] = tot;
	table->value[tot] = a;
	table->tot++;
	return tot;
}

int get_mutex_id(pthread_mutex_t *x) {
	return table_find(&table, (UI)x + 5678901234ULL);
}

int get_cond_id(pthread_cond_t *x) {
	return table_find(&table, (UI)x + 5678901234ULL);
}

int get_thread_id(UI x) {
	return table_find(&table, x);
}

void detector_delete(pthread_mutex_t *mutex) {
	int id = get_mutex_id(mutex);
	
	mutex_set[id] = -1;
	go[id] = -1;
}

int detector_detect(int x) {
	int y = go[x], length = 0;
	
	while (y != -1 && y != x) {
		y = go[y];
		length++;
		//printf("xxxx \n");
	}
	//printf("yyyy %d %d length = %d\n", x, y, length);
	if (y == x) {
		fputs("Deadlock Detected!\n", fout);
		fprintf(fout, "circle length = %d\n", length + 1);	
		return 1;
	} else {
		return 0;
	}
}

void detector_wait(UI thread, pthread_mutex_t *mutex) {
	int tid = get_thread_id(thread), mid = get_mutex_id(mutex);
	go[tid] = mid;
	if (detector_detect(tid)) {
		fprintf(fout, "Caused by thread %d wait pthread_mutex_t %d\n", 
			tid, mid);
		exit(0);
	}
}

void detector_hold(UI thread, pthread_mutex_t *mutex) {
	UI tid = get_thread_id(thread), mid = get_mutex_id(mutex);
	mutex_set[mid] = 1;
	go[mid] = tid;
	go[tid] = -1;
}

int detector_mutex_free(pthread_mutex_t *mutex) {
	int mid = get_mutex_id(mutex);
	return mutex_set[mid] == -1;
}

void detector_init() {
	static int flag = 0;	
	
	old_mutex_lock(&hook_mutex);
	
	if (flag) {
		old_mutex_unlock(&hook_mutex);
		return ;
	}
	flag = 1;
	
	hash_clear(&table);
	old_mutex_unlock(&hook_mutex);
}


int pthread_mutex_lock(pthread_mutex_t *mutex) {
	

	static void *handle = NULL;
	
	if (!handle) {
		fout = fopen("report", "w");
		//fprintf(fout, "No DeadLock detected!\n");
		handle = dlopen("libc.so.6", RTLD_LAZY);
		old_mutex_lock = (MUTEX_LOCK)dlsym(handle, "pthread_mutex_lock");
		old_mutex_unlock = (MUTEX_LOCK)dlsym(handle, "pthread_mutex_unlock");
		
		detector_init();
	}
	
	old_mutex_lock(&hook_mutex);
	if (!detector_mutex_free(mutex)) {
		detector_wait(pthread_self(), mutex);
DEBUG("wait");
		old_mutex_unlock(&hook_mutex);
		return old_mutex_lock(mutex);
	} else {
		detector_hold(pthread_self(), mutex);
DEBUG("get lock\n");
		old_mutex_unlock(&hook_mutex);
		return old_mutex_lock(mutex);
	}
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
	
	old_mutex_lock(&hook_mutex);
	
	detector_delete(mutex);
	
	old_mutex_unlock(&hook_mutex);
	return old_mutex_unlock(mutex);
}


int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
	static void *handle = NULL;
	if (!handle) {
		handle = dlopen("libc.so.6", RTLD_LAZY);
		old_cond_wait = (COND_WAIT)dlsym(handle, "pthread_cond_wait");
	}
	
	old_mutex_lock(&hook_mutex);
	int cid = get_cond_id(cond);
	cpid[cid] = pthread_self();
	cmid[cid] = mutex;
	
	detector_delete(mutex);
	old_mutex_unlock(&hook_mutex);
	
	int ret = old_cond_wait(cond, mutex);
	
	old_mutex_lock(&hook_mutex);
	detector_hold(pthread_self(), mutex);
	old_mutex_unlock(&hook_mutex);
	
	return ret;
}

int pthread_cond_signal(pthread_cond_t *cond) {
	static void *handle = NULL;
	if (!handle) {
		handle = dlopen("libc.so.6", RTLD_LAZY);
		old_cond_signal = (COND_SIGNAL)dlsym(handle, "pthread_cond_signal");
	}
	
	old_mutex_lock(&hook_mutex);
	int cid = get_cond_id(cond);
	pthread_mutex_t* mutex = cmid[cid];
	if (!detector_mutex_free(mutex)) {
		detector_wait(cpid[cid], mutex);
	} else {
		detector_hold(cpid[cid], mutex);
	}
	old_mutex_unlock(&hook_mutex);
	return old_cond_signal(cond);
}

