/**
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * usertimer.c: Implementierung zur Verwaltung von Timern für Benutzer
 */
#include <sys/types.h>
#include <time.h>
#include "vardefine.h"

//------------------------------------------------------------------------------
// Fields
//------------------------------------------------------------------------------
struct timespec startTimes[MAXUSERS];

//------------------------------------------------------------------------------
// Implementations
//------------------------------------------------------------------------------
void startTimer(int userId) {
    clock_gettime(CLOCK_REALTIME, &startTimes[userId]);
}

long getCurrentTimerDurationMillis(int userId) {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);

    return (now.tv_sec - startTimes[userId].tv_sec) * 1000 + (now.tv_nsec - startTimes[userId].tv_nsec) / 1000000;
}
