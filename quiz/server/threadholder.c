/**
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * threadholder.h: Implementierung zuer Verwaltung von Threads.
 */
#include <stdlib.h>
#include <signal.h>
#include "threadholder.h"
#include "../common/util.h"

typedef struct list_item {
    pthread_t threadId;
    struct list_item *next;
} LIST_ITEM;

static LIST_ITEM *first = NULL;

static pthread_t *mainThreadId = NULL;

void registerMainThread(pthread_t threadId) {
    *mainThreadId = threadId;
}

void killMainThread() {
    // TODO This method may must be thread safe! Check it out!
    if(mainThreadId != NULL) {
        // Kill does not mean the thread gets killed immediately, it just sends signals
        pthread_kill(*mainThreadId, SIGTERM);
    }
}

void registerThread(pthread_t threadId) {
    if (first == NULL) {
        first = malloc(sizeof(LIST_ITEM));
        first->threadId = threadId;
        first->next = NULL;
        return;
    }

    LIST_ITEM *lastItem = first;
    while (lastItem->next != NULL) {
        lastItem = lastItem->next;
    }

    LIST_ITEM *newItem = malloc(sizeof(LIST_ITEM));
    newItem->threadId = threadId;
    newItem->next = NULL;
    lastItem->next = newItem;
}

void cancelAllServerThreads() {
    infoPrint("Cancelling all server threads...");

    LIST_ITEM *item = first;
    while (item != NULL) {
        LIST_ITEM *removeCacheItem = item;
        infoPrint("Cancelling thread %lu", item->threadId);
        pthread_kill(item->threadId, 0);
        item = item->next;
        free(removeCacheItem);
    }
    first = NULL;

    infoPrint("Cancelling all server threads... DONE");
}
