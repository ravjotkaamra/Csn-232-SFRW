<h1 align="center">Csn-232-SFRW 💻</h1>
<p>
</p>

> Starve Free Readers-Writers using semaphores

## Author

👤 **Ravjot Singh**

🆔 **19113115**

* Github: [@ravjot](https://github.com/ravjotkaamra)

***

## Introduction

The Readers-Writers problem is a classic concurrency problem in which multiple threads are concurrently reading and writing from a file, data structure or database. When a writer thread is writing or modifying a concurrent data structure, readers should wait until the writer is done.

Two points have to be kept in mind while solving this concurrency issue:

1. When a reader thread enters the critical section, writers have to wait until the readers are done.
2. On the other hand, when a writer thread enters the critical section, readers and writers have to wait until the first writer is done.

So we have to create an algorithm in such a way that takes advantage of the fact that multiple readers can concurrently read from the data structure but also ensure that writers do not starve and get a chance to write on to the data structure.

## Installation

For running this project on Linux we will be needing `GCC` compiler and `make tool` which can be installed with the command:

```bash
sudo apt-get update
sudo apt-get install build-essential
```

Check if installation was successful:

```bash
gcc --version
make --version
```

## Usage

Run the following commands to execute the `main.c` code :

```bash
git clone https://github.com/ravjotkaamra/Csn-232-SFRW.git
cd Csn-232-SFRW
make
./output/main
```

[![asciicast](https://asciinema.org/a/1vWY751JvAJ5aBy2Ej5Xla76V.svg)](https://asciinema.org/a/1vWY751JvAJ5aBy2Ej5Xla76V)

## Main Function

In this section, we will discuss what happens in the main function or, in other words, what is the job of the main thread. The main thread has to perform the following three main tasks.

1. Initialise linked list's head to NULL and various semaphores to proper values to ensure the correctness of the algorithm.

   ```c
   // **global**
   rwlock_t lock;
   int value = 0;
   
   // **main**
   rwlock_init(&lock);
   list_init();
   ```

   

2. Create multiple reader and writer threads as specified by the user in the standard input. 

   ```c
   // thread identifier for readers and writers
   pthread_t pr[num_readers], pw[num_writers];
   
   // array to store threads ids of readers and writers
   int tid_r[num_readers], tid_w[num_writers];
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
   
   ```

   

3. Wait for the completion of all the created threads so that an abrupt ending does not happen. 

   ```c
   // ask the main thread to wait for readers to complete
   for (i = 0; i < num_readers; i++)
       Pthread_join(pr[i], NULL);
   
   // wait for the completion of writer threads
   for (i = 0; i < num_writers; i++)
       Pthread_join(pw[i], NULL);
   ```

   



## Linked List

The concurrent data structure which we will be using is a singly linked list. It is declared in 'include/my_linked_list.h' and defined in 'src/my_linked_list.h'. It provides an interface for initialising head to NULL, inserting to the front of the linked list and getting the sum of all the values stored in the linked list.

```c
// Node Definition: my_linked_list.c
typedef struct __node_t
{
    int val;
    struct __node_t *next;
} node_t;

node_t *head;
```

Following are the various functions which can be performed on a linked list.

```c
// Function Declarations: my_linked_list.h
void list_init();
void list_insert(int value);
int list_get_sum();
```

## Semaphores

We will be using a `struct` called `rwlock_t`, as shown below, for accessing semaphores and the number of readers. The struct `rwlock_t` contains the following fields: 

```c
// Readers-Writer lock: sf_readers_writers.h
typedef struct __rwlock_t
{
    int readers;
    sem_t roomEmpty; // allow ONE writer/MANY readers
    sem_t turnstile; // prevents starvation of writers
    sem_t mutex;     // binary semaphore for accessing readers count

} rwlock_t;
```

Following table defines the function of each field in the `struct rwlock_t`

| rwlock_t          | Function                                                     |
| ----------------- | :----------------------------------------------------------- |
| `int readers`     | an integers which keeps track of the number of readers currently reading the linked list. |
| `sem_t roomEmpty` | a semaphore required for entering the read/write room. Entering the room means getting access to the linked list for either reading or writing. |
| `sem_t turnstile` | a semaphore used by writers to prevent starvation. Whenever a writer wants to write, it acquires the `turnstile` and does not allow any further readers/ writers to enter the room. |
| `sem_t mutex`     | a binary semaphore required for ensuring mutually exclusive access to `readers`. |

## Algorithm

```c
// Readers-Writers Function: sf_readers_writer.h
void rwlock_init(rwlock_t *rw);
void rwlock_acquire_readlock(rwlock_t *rw);
void rwlock_release_readlock(rwlock_t *rw);
void rwlock_acquire_writelock(rwlock_t *rw);
void rwlock_release_writelock(rwlock_t *rw);
```

The readers and writers acquire the reader-writer lock `rwlock_t` in a different manner.

### Readers

##### Acquire

The reader first tries to acquire the `turnstile`. Two possibilities can happen, 

- If no writer is active currently, eventually, all readers will acquire the `turnstile` and release it immediately so that no readers have to enter the blocked state.
- Else readers will wait on the `turnstile` to get released by the writer. The writer will signal `turnstile` only when it is done with its critical section.

The next step for readers is to acquire the `mutex` and increment the `readers` count by one. If it is the first reader, it must acquire the `roomEmpty` semaphore for entering the room for reading. However, if it is not the first reader, it will enter the room without acquiring the `roomEmpty` semaphore. 

```c
void rwlock_acquire_readlock(rwlock_t *rw)
{
    Sem_wait(&rw->turnstile);
    Sem_post(&rw->turnstile);

    Sem_wait(&rw->mutex);
    rw->readers++;
    
    if (rw->readers == 1)
        Sem_wait(&rw->roomEmpty);
 
    Sem_post(&rw->mutex);
}
```



##### Release

Finally, when readers are done, they will decrement the *readers* count by acquiring the `mutex` and releasing the `roomEmpty` semaphore if it is the last reader.

```c
void rwlock_release_readlock(rwlock_t *rw)
{
    Sem_wait(&rw->mutex);
    rw->readers--;
    
    if (rw->readers == 0)
        Sem_post(&rw->roomEmpty);
    
    Sem_post(&rw->mutex);
}
```



##### Errors?

What if a writer is already in the room and the first reader cannot acquire `roomEmpty` semaphore? Well, it will go to the blocked state and wait for the writer to wake it up. 

What if the second reader wants to enter the room after the first reader fails? It does not try to acquire the `roomEmpty` semaphore and may enter the room while the writer is still writing. Fortunately, that will *not* happen because when the first reader goes to sleep, it takes the `mutex` along with it, which prevents the second reader from incrementing the `readers` count and entering the critical section.

### Writers

##### Acquire

The writer, on the other hand, works differently. It will acquire the `turnstile` to indicate that no further readers are allowed to enter to prevent starvation. After acquiring `turnstile`, the writer acquires `roomEmpty` semaphore and enters the *critical* section. 

```c
void rwlock_acquire_writelock(rwlock_t *rw)
{
    Sem_wait(&rw->turnstile);
    Sem_wait(&rw->roomEmpty);
}
```

##### Release

When it is done, the writer first releases the `turnstile` so that readers/writers can continue with their work and then releases `roomEmpty` semaphore.

```c
void rwlock_release_writelock(rwlock_t *rw)
{
    Sem_post(&rw->turnstile);
    Sem_post(&rw->roomEmpty);
}
```



##### Errors?

If writers, just like readers, immediately released the `turnstile` instead of holding it, the writers will starve because readers will keep on entering the room for reading. Hence writers will not get a chance until all the readers stop reading.



## Worker Threads

Worker threads in this context are readers and writers, which perform the reading and writing task in the following manner.

##### Writer Threads

```c
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
```



##### Reader Threads

```c
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
```

## References

Arpaci-Dusseau, R.H. and Arpaci-Dusseau, A.C., 2018. Operating systems: Three easy pieces. Arpaci-Dusseau Books LLC.

## License

```
MIT License

Copyright (c) [2021] [Ravjot Singh]
```
