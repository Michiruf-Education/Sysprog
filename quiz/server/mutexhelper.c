/**
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * mutexhelper.h: Implementierung für mutex functions.
 */
#include "mutexhelper.h"

//------------------------------------------------------------------------------
// Implementations
//------------------------------------------------------------------------------
int mutexInit(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
    return pthread_mutex_init(mutex, attr);
}

int mutexLock(pthread_mutex_t *mutex) {
    // NOTE If the next line is enabled, the catalog module sends a malformed utf-8 encoded string to the
    // loader. We do not now why this happens... (maybe because of the fork?)
    //pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    return pthread_mutex_lock(mutex);
}

int mutexUnlock(pthread_mutex_t *mutex) {
    int result = pthread_mutex_unlock(mutex);
    // NOTE Disabled because comment above
    //pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    return result;
}
