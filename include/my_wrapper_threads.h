#ifndef __wrapper_threads_h__
#define __wrapper_threads_h__

#include <pthread.h>
#include <assert.h>
#include <sched.h>

#ifdef __linux__
#include <semaphore.h>
#endif

#define Pthread_create(thread, attr, start_routine, arg) assert(pthread_create(thread, attr, start_routine, arg) == 0);
#define Pthread_join(thread, value_ptr)                  assert(pthread_join(thread, value_ptr) == 0);


#ifdef __linux__
#define Sem_init(sem, value)                             assert(sem_init(sem, 0, value) == 0);
#define Sem_post(sem)                                    assert(sem_post(sem) == 0);
#define Sem_wait(sem)                                    assert(sem_wait(sem) == 0);

#endif // __linux__

#endif // __wrapper_threads_h__
