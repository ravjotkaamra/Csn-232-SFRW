#ifndef __sf_read_write_h__
#define __sf_read_write_h__

#include <semaphore.h>

typedef struct __rwlock_t
{
    int readers;
    sem_t roomEmpty; // allow ONE writer/MANY readers
    sem_t turnstile; // prevents starvation of writers
    sem_t mutex;     // binary semaphore for accessing readers count

} rwlock_t;

void rwlock_init(rwlock_t *rw);
void rwlock_acquire_readlock(rwlock_t *rw);
void rwlock_release_readlock(rwlock_t *rw);
void rwlock_acquire_writelock(rwlock_t *rw);
void rwlock_release_writelock(rwlock_t *rw);

#endif // __sf_read_write_h__