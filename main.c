#include <semaphore.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "thread_data.h"
#include "cuckoo.h"

#define QUIT -1
#define LOCAL_INT_ADDRESS 100
#define STRING_MESSAGE 101

// this is just a test service fun for thread data, base for cuckoo_alloc
int thread_function_1(thread_data *td, int message_type, void *data)
{
	static char *cuckoo_allocated_place = NULL;
	static int cuckoo_allocated_count = 0;
	static int *hs;

	switch(message_type)
	{
		case 1:
			printf("Message : 1\n");
			break;
		case LOCAL_INT_ADDRESS:
			printf("Got local of other thread : %d\n", *((int*)data));
			hs = data;
			*hs = 10;
		break;
		case STRING_MESSAGE:
			printf("Got a string : %s\n", (char*)data);
		break;
		
		case -1:
			printf("Quitting thread!\n");
			td->running = 0;
			break;
		default:
			printf("Other message type (%d)\n", message_type);
			break;
	}
	
	return 0;
}

void test_cuckoo()
{
	printf("CUCKOO!");

	cuckoo_nest *nest = create_cuckoo_nest();
	cuckoo_alloc_request *req = create_cuckoo_alloc_request(nest, 1024);
	
	try_cuckoo_alloc(nest, req);
	
	char *ptr = (char*)wait_cuckoo_alloc_confirmation(req);
	destroy_cuckoo_alloc_request(req);
	
	printf("Allocated : ptr = %p");
	memset(ptr, 'a', 512);
	ptr[512] = '\0';
	
	printf("Under ptr: '%s'", ptr);
	
	destroy_cuckoo_nest(nest);
}

// this is just a test for thread data, the base for cuckoo alloc
int test_thread_data()
{
	thread_data *td1 = thread_data_new(thread_function_1);
	
	int some_var = 43;

	thread_data_run(td1);	
	
	thread_data_insert_message(td1, 1, NULL);
	thread_data_insert_message(td1, 2, NULL);	
	thread_data_insert_message(td1, LOCAL_INT_ADDRESS, &some_var);
	
	// wait until nasty thread changes our local
	while(some_var == 43);	
	printf("some_var now:  %d\n", some_var);
	thread_data_insert_message(td1, STRING_MESSAGE, "Hola!");

	thread_data_insert_message(td1, -1, NULL);
	
	thread_data_join(td1);
		
	return 0;
}

int main()
{
	test_thread_data();
	test_cuckoo();
}
