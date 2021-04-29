#include "../include/sf_read_write.h"
#include "../include/my_wrapper_threads.h"
#include <semaphore.h>


void rwlock_init(rwlock_t *rw)
{
    rw->readers = 0;
    Sem_init(&rw->roomEmpty, 1);
    Sem_init(&rw->turnstile, 1);
    Sem_init(&rw->lock, 1);
}

void rwlock_acquire_readlock(rwlock_t *rw)
{
    // acquire turnstile and release immediately.
    // it blocks the upcoming reader threads,
    // if some writer is waiting for writing
    Sem_wait(&rw->turnstile);
    Sem_post(&rw->turnstile);

    // acquire lock semaphore for
    // accessing no of readers
    Sem_wait(&rw->lock);
    rw->readers++;

    // acquire roomEmpty semaphore only,
    // if the first readers arrives
    if (rw->readers == 1)
    {
        Sem_wait(&rw->roomEmpty);
    }

    // release the lock
    Sem_post(&rw->lock);
}

void rwlock_release_readlock(rwlock_t *rw)
{
    // acquire lock semaphore for
    // accessing no of readers
    Sem_wait(&rw->lock);
    rw->readers--;

    // when last reader leaves, then
    // release the roomEmpty semaphpore
    if (rw->readers == 0)
    {
        Sem_post(&rw->roomEmpty);
    }

    // release the lock
    Sem_post(&rw->lock);
}

void rwlock_acquire_writelock(rwlock_t *rw)
{
    Sem_wait(&rw->turnstile);
    Sem_wait(&rw->roomEmpty);
}

void rwlock_release_writelock(rwlock_t *rw)
{
    Sem_post(&rw->turnstile);
    Sem_post(&rw->roomEmpty);
}