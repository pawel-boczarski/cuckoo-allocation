all: main.c thread_data.c thread_data.h cuckoo.c cuckoo.h
	gcc -o main main.c thread_data.c cuckoo.c -lpthread -g
