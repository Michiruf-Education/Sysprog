/**
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * usertimer.c: Implementierung zur Verwaltung von Timern für Benutzer
 */
#include <sys/types.h>
#include "vardefine.h"

//------------------------------------------------------------------------------
// Fields
//------------------------------------------------------------------------------
timer_t timers[MAXUSERS];

//------------------------------------------------------------------------------
// Implementations
//------------------------------------------------------------------------------
void createTimer(int userId) {

}

void startTimer(int userId) {
    timer_create();
}

int getCurrentTimerDuration(int userId) {

}

void stopTimer(int userId) {

}
