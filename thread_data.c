#include <semaphore.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "thread_data.h"

void* thread_data_main_impl(void *data)
{
	thread_data *td = (thread_data*)data;
	printf("I'm a thread!\n");
	while(td->running)
	{
		sem_wait(&td->signal_semaphore);
		td->service_fun(td, td->message_type, td->data);
		sem_post(&td->operation_semaphore);
	}
	
	return NULL;
}

thread_data *thread_data_new(int (*service_fun)(struct _thread_data *, int, void *))
{
	thread_data *td = calloc(1, sizeof *td);
	if(0 != sem_init(&td->signal_semaphore, 0, 0))
	{
		printf("ERROR initializing thread semaphore : %d '%s'\n", errno, strerror(errno));
		free(td);
		return NULL;
	}	
	
	if(0 != sem_init(&td->operation_semaphore, 0, 1))
	{
		printf("ERROR initializing operation semaphore : %d '%s'\n", errno, strerror(errno));
		free(td);
		return NULL;
	}
	
	td->service_fun = service_fun;

	return td;
}

void thread_data_run(thread_data *td)
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	td->running = 1;

	printf("Create...\n");
	if(0 != pthread_create(&td->thread_id, &attr, thread_data_main_impl, td))
	{
		printf("Thread init failed : %d '%s'\n", errno, strerror(errno));
	}	
}

void thread_data_insert_message(thread_data *td, int message_type, void *message_data)
{
	sem_wait(&td->operation_semaphore);
	td->message_type = message_type;
	td->data = message_data;
	sem_post(&td->signal_semaphore);
}

void thread_data_join(thread_data *td)
{
	pthread_join(td->thread_id, NULL);
}
