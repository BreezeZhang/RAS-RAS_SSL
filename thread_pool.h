#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#define return_if_null(p) if((p) == NULL)\
        {printf("%s:%d Warning: failed.\n",\
            __func__, __LINE__); return;}
#define return_null_if_fail(p) if((p) == NULL)\
        {printf("%s:%d Warning: failed.\n",\
            __func__, __LINE__); return NULL;}
#define return_val_if_fail(p, ret) if((p) == NULL)\
        {printf("%s:%d Warning: failed.\n", \
            __func__, __LINE__); return (ret);}

typedef void (*threadpool_task_func_t)(void *);
typedef	struct threadpool	threadpool_t;
threadpool_t*	threadpool_create(int max_thr_num);
int	threadpool_destroy(threadpool_t *thiz);
int	threadpool_add_task(threadpool_t *thiz, threadpool_task_func_t task_func, void *arg);

#endif
