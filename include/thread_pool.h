
#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <windows.h> 
#include <stdlib.h>

#define THREAD_NUM_MIN	4
#define THREAD_NUM_MAX	30	

typedef void (*work_fn)(void *arg);

#define NODELAY		(1 << 0)

typedef struct task_t{
	void *arg;
	work_fn func;
	struct task_t *next;
}task_t;

enum {
	POOL_IDLE = 0,
	POOL_BUSY,
	POOL_DESTROYING,
};
typedef struct thread_pool_t{
	int status;
	unsigned int nr_tasks;
	unsigned int nr_threads;
	unsigned int nr_waiting_threads;
	CRITICAL_SECTION cs; /* lock for this struct */
	struct task_t *task_list;
	HANDLE	task_event; /* used for wakeup threads */
	HANDLE thread_handler[THREAD_NUM_MAX];
}thread_pool_t;

int init_pool(thread_pool_t *pool, unsigned int thread_num);
int add_task(thread_pool_t *pool, work_fn func, void *arg, int flags);
int add_thread(thread_pool_t *pool, unsigned thread_num); /* negative to remove thread */
int destroy_pool(thread_pool_t *pool); 

#endif
