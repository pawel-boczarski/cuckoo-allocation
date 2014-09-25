#include "thread_data.h"

#define CUCKOO_NEST_ALLOC 500
#define DESTROY_NEST -1
//#define CUCKOO_NEST_ALLOC_CONFIRM 500

#include "cuckoo.h"

static int
cuckoo_nest_thread(thread_data *td, int message_type, void *data)
{
	switch(message_type)
	{
		case CUCKOO_NEST_ALLOC:
		{
			cuckoo_alloc_request *req = (cuckoo_alloc_request*)data;
			req->data = alloca(req->how_much);
			printf("CUCKOO! allocated %d bytes at %p\n", req->how_much, req->data);
			sem_post(&req->ready);
			while(td->running); /* cuckoo needs the allocated variable so function can't exit */
			printf("CUCKOO has left the loop\n");
		}
		break;
		case DESTROY_NEST:
			printf("CUCKOO: Quitting thread!\n");
			td->running = 0;
			break;
		default:
			printf("CUCKOO: Other message than CUCKOO_NEST_ALLOC and -1\n");
			break;
	}
	
	return 0;
}

cuckoo_nest*
create_cuckoo_nest()
{
	cuckoo_nest *nest = calloc(1, sizeof *nest);
	nest->nest = thread_data_new(cuckoo_nest_thread);
	thread_data_run(nest->nest);
	return nest;
}

/**
 * The current price is one byte per egg ~~
 */
cuckoo_alloc_request*
create_cuckoo_alloc_request(cuckoo_nest *cuckoo_nest, int numberOfEggs)
{
	cuckoo_alloc_request *req = calloc(1, sizeof *req);
	req->how_much = numberOfEggs;
	if(-1 == sem_init(&req->ready, 0, 0)) {
		printf("ERROR initializing request semaphore : %d '%s'\n", errno, strerror(errno));
		free(req);
		return NULL;
	}
	return req;
}

void
try_cuckoo_alloc(cuckoo_nest *nest, cuckoo_alloc_request *alloc_spec)
{
	thread_data_insert_message(nest->nest, CUCKOO_NEST_ALLOC, alloc_spec);
}

void*
wait_cuckoo_alloc_confirmation(cuckoo_alloc_request *req)
{
	sem_wait(&req->ready);
	return req->data;
}

void
destroy_cuckoo_nest(cuckoo_nest *nest)
{
	nest->nest->running = 0;
	thread_data_insert_message(nest->nest, DESTROY_NEST, NULL);
}

void
destroy_cuckoo_alloc_request(cuckoo_alloc_request *req)
{
	sem_destroy(&req->ready);
	free(req);
}
