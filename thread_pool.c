#include "thread_pool.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>



typedef	struct	taskqueue{
	int	head;
	int	tail;
	threadpool_task_func_t	*task_func;	/* work func */
	void	**arg;
}taskqueue_t;

struct	threadpool{
	int			max_thr_num;
	pthread_t		*thread_id; 	/*如果不需要destroy线程，也可以取消*/
	taskqueue_t		*task_queue;
	pthread_cond_t		cond;
	pthread_mutex_t		mutex_cond;
	pthread_mutex_t		mutex_queue;
};

static void* threadpool_excute_task(void *arg);

threadpool_t*	threadpool_create(int max_thr_num)
{
	int	i = 0;
	pthread_t	td;
	threadpool_t 	*tpool;
	return_null_if_fail(tpool = malloc(sizeof (struct threadpool)));
	tpool->max_thr_num = max_thr_num;
	return_null_if_fail(tpool->thread_id = malloc(max_thr_num*sizeof(pthread_t)));
	return_null_if_fail(tpool->task_queue = malloc(sizeof(struct taskqueue)));
	return_null_if_fail(tpool->task_queue->arg = malloc(max_thr_num*sizeof(void *)));
	return_null_if_fail(tpool->task_queue->task_func = malloc(max_thr_num*sizeof(threadpool_task_func_t)));
	tpool->task_queue->head = tpool->task_queue->tail = 0;
	if (pthread_mutex_init(&tpool->mutex_queue, NULL) != 0)
		return NULL;
	if (pthread_mutex_init(&tpool->mutex_cond, NULL) != 0)
		return NULL;
	if (pthread_cond_init(&tpool->cond, NULL) != 0)
		return NULL;
	while (max_thr_num > 0) {
		if (pthread_create(&td, NULL, threadpool_excute_task, (void *) tpool) == 0) {
			tpool->thread_id[i++] = td;
			--max_thr_num;
			continue;
		}
		threadpool_destroy(tpool);
		tpool = NULL;
		break;
	}
	return tpool;
}

int	threadpool_add_task(threadpool_t *thiz, threadpool_task_func_t task_func, void *arg)
{
	int index;
	index = thiz->task_queue->tail; 
	return_if_null(thiz);
	pthread_mutex_lock(&thiz->mutex_queue);
	if((index+1)%thiz->max_thr_num == thiz->task_queue->head)
	{
		pthread_mutex_unlock(&thiz->mutex_queue);
		return -1;
	}	
	thiz->task_queue->arg[index] = arg;
	thiz->task_queue->task_func[index] = task_func;
	thiz->task_queue->tail = (index+1)%thiz->max_thr_num;
	pthread_mutex_unlock(&thiz->mutex_queue);
	
	pthread_cond_signal(&thiz->cond);
	return 0;
}

int threadpool_stop_all_task(threadpool_t *thiz)
{
	pthread_t i;
	return_val_if_fail(thiz, -1);

	for (i = 0; i <= thiz->max_thr_num; ++i) {
		pthread_cancel(thiz->thread_id[i]);
		pthread_join(thiz->thread_id[i], NULL);
	}

	return 0;
}
int	threadpool_destroy(threadpool_t *thiz)
{
	return_val_if_fail(thiz, -1);
	threadpool_stop_all_task(thiz);
	free(thiz->task_queue->arg);
	free(thiz->task_queue->task_func);
	free(thiz->task_queue);
	free(thiz->thread_id);
	pthread_mutex_destroy(&thiz->mutex_queue);
	pthread_mutex_destroy(&thiz->mutex_cond);
	pthread_cond_destroy(&thiz->cond);
	thiz = NULL;
	return 0;
}

void* threadpool_excute_task(void *arg)
{	
	
	int index = 0;
	void *tmp_arg = NULL;
	threadpool_t	*tpool;
	tpool = (threadpool_t*)arg;
	return_if_null(tpool);
	threadpool_task_func_t	task_func;

	while(1)
	{
		pthread_mutex_lock(&tpool->mutex_queue);
		index = tpool->task_queue->head;
		if(index == tpool->task_queue->tail)
		{
				pthread_mutex_unlock(&tpool->mutex_queue);
				pthread_mutex_lock(&tpool->mutex_cond);
				pthread_cond_wait(&tpool->cond, &tpool->mutex_cond); //等待线程调度
				pthread_mutex_unlock(&tpool->mutex_cond);
		}
		else
		{
			task_func = tpool->task_queue->task_func[index];
			tmp_arg = tpool->task_queue->arg[index];
			tpool->task_queue->head = (index+1) % tpool->max_thr_num;
			pthread_mutex_unlock(&tpool->mutex_queue);
			task_func(tmp_arg);
			printf("process end");
		}
	}
}
