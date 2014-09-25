#ifndef __THREAD_DATA_H__
#define __THREAD_DATA_H__

#include <semaphore.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct _thread_data
{
	pthread_t thread_id;
	int running;
	sem_t signal_semaphore;
	sem_t operation_semaphore;
	int message_type;
	void *data;
	int (*service_fun)(struct _thread_data *, int, void *);
};

typedef struct _thread_data thread_data;

thread_data *thread_data_new(int (*service_fun)(struct _thread_data *, int, void *));

void thread_data_run(thread_data *td);

void thread_data_insert_message(thread_data *td, int message_type, void *message_data);

void thread_data_join(thread_data *td);

#endif /* __THREAD_DATA_H__ */
