#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cuckoo.h"

int main()
{
	printf("CUCKOO!");

	cuckoo_nest *nest = create_cuckoo_nest(); /* create a thread our eggs will hatch */

	cuckoo_alloc_request *req = create_cuckoo_alloc_request(nest, 1024); /* 1024 byte-eggs to be laid at the cuckoo nest, take one down and... */
	try_cuckoo_alloc(nest, req); /* send a cuckoo to lay the eggs */
	
	char *ptr = (char*)wait_cuckoo_alloc_confirmation(req); /* wait until eggs hatch */
	destroy_cuckoo_alloc_request(req);
	
	printf("Allocated : ptr = %p\n", ptr);
	memset(ptr, 'a', 512);
	ptr[512] = '\0';
	
	printf("Under ptr: '%s'\n", ptr);
	
	destroy_cuckoo_nest(nest); /* Nest, I say to thee, fall off the tree so that eggs are deallocated! */
}
