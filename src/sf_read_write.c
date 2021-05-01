#include "../include/sf_read_write.h"
#include "../include/my_wrapper_threads.h"
#include <semaphore.h>

void rwlock_init(rwlock_t *rw)
{
    rw->readers = 0;
    Sem_init(&rw->roomEmpty, 1);
    Sem_init(&rw->turnstile, 1);
    Sem_init(&rw->mutex, 1);
}

void rwlock_acquire_readlock(rwlock_t *rw)
{
    // acquire turnstile and release immediately.

    // if a writer have already acquired the turnstile
    // then readers go to blocked state and wait
    // until the writer releases the turnstile
    Sem_wait(&rw->turnstile);
    Sem_post(&rw->turnstile);

    // acquire mutex for
    // accessing no of readers
    Sem_wait(&rw->mutex);
    rw->readers++;

    // acquire roomEmpty semaphore only,
    // if the first readers arrives
    if (rw->readers == 1)
    {
        Sem_wait(&rw->roomEmpty);
    }

    // release the lock
    Sem_post(&rw->mutex);
}

void rwlock_release_readlock(rwlock_t *rw)
{
    // acquire mutex for
    // accessing no of readers
    Sem_wait(&rw->mutex);
    rw->readers--;

    // when last reader leaves, then
    // release the roomEmpty semaphore
    if (rw->readers == 0)
    {
        Sem_post(&rw->roomEmpty);
    }

    // release the mutex
    Sem_post(&rw->mutex);
}

void rwlock_acquire_writelock(rwlock_t *rw)
{
    // acquire the turnstile lock so
    // no new readers/writers can
    // enter the room
    Sem_wait(&rw->turnstile);

    // acquire the roomEmpty lock
    // to start writing to the
    // linkedlist
    Sem_wait(&rw->roomEmpty);
}

void rwlock_release_writelock(rwlock_t *rw)
{
    Sem_post(&rw->turnstile);
    Sem_post(&rw->roomEmpty);
}