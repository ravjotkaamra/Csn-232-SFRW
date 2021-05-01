#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/my_wrapper_threads.h"
#include "../include/my_linked_list.h"
#include "../include/sf_read_write.h"

// number of times a single thread reads or writes
int num_loops;

// value which writer writes to the linked list
int value = 0;

// reader-writer lock for synchronization
rwlock_t lock;

void *writer(void *arg)
{
	int i;
	int tid_w = *(int *)arg;
	for (i = 0; i < num_loops; i++)
	{
		// acquire the writelock
		rwlock_acquire_writelock(&lock);

		// critical section
		value++;
		list_insert(value);
		printf("%-4d %-10s %-20s %8d\n", tid_w, "Writer", "insert to front", value);

		// release the writelock
		rwlock_release_writelock(&lock);
	}
	return NULL;
}

void *reader(void *arg)
{
	int i;
	int tid_r = *(int *)arg;
	for (i = 0; i < num_loops; i++)
	{
		// acquire the readlock
		rwlock_acquire_readlock(&lock);

		// critical section
		int sum = list_get_sum();
		printf("%-4d %-10s %-20s %8d\n", tid_r, "Reader", "get sum of elements", sum);

		// release the readlock
		rwlock_release_readlock(&lock);
	}
	return NULL;
}

int main()
{

	int num_readers;
	int num_writers;

	printf("Enter number of Writer Threads: ");
	scanf("%d", &num_writers);

	assert(num_writers >= 0);

	printf("Enter number of Reader Threads: ");
	scanf("%d", &num_readers);

	assert(num_readers >= 0);

	printf("Enter number of iterations for each thread: ");
	scanf("%d", &num_loops);

	assert(num_loops > 0);

	// thread identifier for readers and writers
	pthread_t pr[num_readers], pw[num_writers];

	// array to store threads ids of readers and writers
	int tid_r[num_readers], tid_w[num_writers];

	// initialise the semaphores and readers counter
	// for ensuring correctness of synchronisation
	rwlock_init(&lock);

	// initialise the linked list's head to NULL
	list_init();

	printf("%-4s %-10s %-20s %8s\n", "TID", "Thread", "Task", "Value");

	int i;
	for (i = 0; i < num_readers; i++)
	{
		tid_r[i] = i;
		Pthread_create(&pr[i], NULL, reader, &tid_r[i]);
	}

	for (i = 0; i < num_writers; i++)
	{
		tid_w[i] = i;
		Pthread_create(&pw[i], NULL, writer, &tid_w[i]);
	}

	// ask the main thread to wait for readers to complete
	for (i = 0; i < num_readers; i++)
		Pthread_join(pr[i], NULL);

	// wait for the completion of writer threads
	for (i = 0; i < num_writers; i++)
		Pthread_join(pw[i], NULL);

	return 0;
}