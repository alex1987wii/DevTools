#include "thread_pool.h"
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#define POOL_DEBUG	printf
#define POOL_ERROR	printf

static inline void pool_show(thread_pool_t *pool)
{
	assert(pool);
	EnterCriticalSection(&pool->cs);
	POOL_DEBUG("pool->nr_tasks = %u\n", pool->nr_tasks);
	POOL_DEBUG("pool->nr_threads = %u\n", pool->nr_threads);
	POOL_DEBUG("pool->nr_waiting_threads = %u\n", pool->nr_waiting_threads);
	int i = 0;
	struct task_t *p_task = pool->task_list;
	while(p_task){
		p_task = p_task->next;
		++i;
	}
	POOL_DEBUG("caculate nr_tasks = %d\n", i);
	assert(i == pool->nr_tasks);
	LeaveCriticalSection(&pool->cs);
	
}
DWORD WINAPI routine(LPVOID lpParamter)
{
	thread_pool_t *pool = (thread_pool_t *)lpParamter;
	struct task_t *p_task;
	while(1){
		EnterCriticalSection(&pool->cs);
		while(pool->status == POOL_DESTROYING || pool->nr_tasks <= 0){
			ResetEvent(pool->task_event);
			LeaveCriticalSection(&pool->cs);
			WaitForSingleObject(pool->task_event, INFINITE);
			EnterCriticalSection(&pool->cs);
		}
		pool->nr_waiting_threads--;
		if(pool->status == POOL_DESTROYING){
			pool->nr_threads--;
			LeaveCriticalSection(&pool->cs);
			break;
		}
		
		p_task = pool->task_list;
		pool->task_list = p_task->next;
		p_task->next = NULL;
		pool->nr_tasks--;
		LeaveCriticalSection(&pool->cs);

		p_task->func(p_task->arg);
		free(p_task);

		EnterCriticalSection(&pool->cs);
		pool->nr_waiting_threads++;
		//pool_show(pool);
		LeaveCriticalSection(&pool->cs);
	}
	return 0;
}

int init_pool(thread_pool_t *pool, unsigned thread_num)
{
	if(pool == NULL || thread_num < THREAD_NUM_MIN || thread_num > THREAD_NUM_MAX)
		return -EINVAL;
	InitializeCriticalSection(&pool->cs);
	EnterCriticalSection(&pool->cs);
	pool->status = POOL_IDLE;
	pool->nr_tasks = 0;
	pool->task_list = NULL;
	pool->nr_threads = 0;
	pool->nr_waiting_threads = 0;
	int i;
	/* clear thread_handler */
	for(i = 0; i < THREAD_NUM_MAX; ++i){
		pool->thread_handler[i] = NULL;
	}
	/* create task_event */
	pool->task_event = CreateEvent(NULL, TRUE, FALSE, NULL);

	/* create threads */
	for(i = 0; i < thread_num; ++i){
		pool->thread_handler[i] = CreateThread(NULL, 0, routine, (LPVOID)pool, 0, NULL);
		if(pool->thread_handler[i] == NULL){
			destroy_pool(pool);
			LeaveCriticalSection(&pool->cs);
			return GetLastError();
		}
		pool->nr_threads++;
		pool->nr_waiting_threads++;
	}
	//pool_show(pool);
	LeaveCriticalSection(&pool->cs);
	return 0;
}

int destroy_pool(thread_pool_t *pool)
{
	struct task_t *p; 
	EnterCriticalSection(&pool->cs);
	/*destroy task_list */
	p = pool->task_list;
	while(p){
		pool->task_list = p->next;
		free(p);
		p = pool->task_list;
	}
	pool->nr_tasks = 0;

	pool->status = POOL_DESTROYING;
	SetEvent(pool->task_event);
	LeaveCriticalSection(&pool->cs);
	return 0;
}

int add_task(thread_pool_t *pool, work_fn func, void *arg, int flags)
{
	assert(pool && func);
	int retval = 0;
	struct task_t **p;
	struct task_t *p_task = malloc(sizeof(struct task_t));
	if(p_task == NULL)
		return -ENOMEM;
	p_task->arg = arg;
	p_task->func = func;
	p_task->next = NULL;

	EnterCriticalSection(&pool->cs);
	if(pool->status == POOL_DESTROYING){
		retval = -EPERM;
		goto error1;
	}
	while(flags == NODELAY && pool->nr_waiting_threads <= pool->nr_tasks){
		retval = add_thread(pool, 1);
		if(retval <= 0)
			goto error1;
	}
	p = &pool->task_list;
	while(*p)
		p = &(*p)->next;
	*p = p_task;
	pool->nr_tasks++;

	//pool_show(pool);
	SetEvent(pool->task_event);
	LeaveCriticalSection(&pool->cs);
	return 0;
error1:
	LeaveCriticalSection(&pool->cs);
	free(p_task);
	POOL_ERROR("add_task failed, retval = %d\n", retval);
	return retval;
}

int add_thread(thread_pool_t *pool, unsigned thread_num)
{
	int retval = 0;
	assert(pool && thread_num);
	EnterCriticalSection(&pool->cs);
	if(pool->status == POOL_DESTROYING){
		retval = -EPERM;
		goto error1;
	}
	if(thread_num + pool->nr_threads > THREAD_NUM_MAX){
		retval = -EPERM;
		goto error1;
	}
	int i;
	for(i = 0; i < thread_num; ++i){
		pool->thread_handler[pool->nr_threads] = CreateThread(NULL,
				0,
				routine,
				(LPVOID)pool,
				0,
				NULL);
		if(pool->thread_handler[pool->nr_threads] == NULL){
			retval = i;
			goto error1;
		}
		pool->nr_threads++;
		pool->nr_waiting_threads++;
	}
	LeaveCriticalSection(&pool->cs);
	return i;
error1:
	LeaveCriticalSection(&pool->cs);
	return retval;
}
