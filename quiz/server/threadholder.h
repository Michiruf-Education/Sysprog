/**
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * threadholder.h: Header zur Verwaltung von Threads
 */
#ifndef THREADHOLDER_H
#define THREADHOLDER_H

#include <pthread.h>

void registerMainThread(pthread_t threadId);

void cancelMainThread();

void registerThread(pthread_t threadId);

void cancelAllServerThreads();

#endif
