/**
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * mutexhelper.h: Header für mutex functions.
 */
#ifndef MUTEXHELPER_H
#define MUTEXHELPER_H

#include <pthread.h>

int mutexInit(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);

int mutexLock(pthread_mutex_t *mutex);

int mutexUnlock(pthread_mutex_t *mutex);

#endif
