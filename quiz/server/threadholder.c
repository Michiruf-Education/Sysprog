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

LIST_ITEM *first = NULL;

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
        infoPrint("Cancelling thread %p", item->threadId);
        pthread_kill(item->threadId, 0);
        item = item->next;
        free(removeCacheItem);
    }
    first = NULL;

    infoPrint("Cancelling all server threads... DONE");
}
