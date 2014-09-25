#ifndef __CUCKOO_H__
#define __CUCKOO_H__

#include "thread_data.h"

typedef struct _cuckoo_alloc_request
{
	int how_much;
	void *data;
	sem_t ready;
} cuckoo_alloc_request;

typedef struct _cuckoo_nest {
	thread_data *nest;
} cuckoo_nest;


cuckoo_nest*
create_cuckoo_nest();

/**
 * The current price is one byte per egg ~~
 */
cuckoo_alloc_request*
create_cuckoo_alloc_request(cuckoo_nest *cuckoo_nest, int numberOfEggs);

void
try_cuckoo_alloc(cuckoo_nest *nest, cuckoo_alloc_request *alloc_spec);

void*
wait_cuckoo_alloc_confirmation(cuckoo_alloc_request *alloc_spec);

void
destroy_cuckoo_nest();

void
destroy_cuckoo_alloc_request(cuckoo_alloc_request *req);

#endif /* __CUCKOO_H__ */
