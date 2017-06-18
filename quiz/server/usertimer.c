/**
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * usertimer.c: Implementierung zur Verwaltung von Timern für Benutzer
 */
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include "../common/util.h"
#include "vardefine.h"

//------------------------------------------------------------------------------
// Method pre-declarations
//------------------------------------------------------------------------------
static void callTimerCallback(int sig, siginfo_t *si, void *uc);

//------------------------------------------------------------------------------
// Fields
//------------------------------------------------------------------------------
static timer_t timers[MAXUSERS] = {NULL};

static void (*timerCallbacks[MAXUSERS])(int);

//------------------------------------------------------------------------------
// Implementations
//------------------------------------------------------------------------------
int startTimer(int userId, int durationSeconds, void (*timerCallback)(int)) {
    // Store the timer callback for later use
    timerCallbacks[userId] = timerCallback;

    // If the timer was not created yet, start it
    if (timers[userId] == NULL) {
        // Setup the signal action (calling the callback handler)
        struct sigaction action;
        action.sa_flags = SA_SIGINFO;
        action.sa_sigaction = callTimerCallback;
        sigemptyset(&action.sa_mask);
        if (sigaction(SIGRTMIN, &action, NULL) < 0) {
            errorPrint("Unable to create signal action for timer for user %d!", userId);
            return -1;
        }
        debugPrint("Created signal action for timer for user %d", userId);

        // Create the event for the timer callback
        struct sigevent event;
        event.sigev_signo = SIGRTMIN;
        event.sigev_notify = SIGEV_SIGNAL;
        event.sigev_value.sival_int = userId;
        if (timer_create(CLOCK_REALTIME, &event, &timers[userId]) < 0) {
            errorPrint("Unable to create timer with sigevent for user %d!", userId);
            return -2;
        }
        debugPrint("Created timer with sigevent for user %d", userId);
    }

    // Start the timer
    struct itimerspec countdown = {0}; // TODO Try to remove "= {0}"
    countdown.it_value.tv_sec = durationSeconds;
    if (timer_settime(timers[userId], 0, &countdown, NULL) < 0) {
        errorPrint("Unable to start timer with sigevent for user %d!", userId);
        return -3;
    }

    debugPrint("Started timer for user %d", userId);
    return 0;
}

int stopTimer(int userId) {
    // Stop the timer by removing the countdown
    struct itimerspec countdown = {0};
    if (timer_settime(timers[userId], 0, &countdown, NULL) < 0) {
        errorPrint("Unable to stop the timer for user %d!", userId);
        return -1;
    }

    debugPrint("Stopped timer for user %d", userId);
    return 0;
}

long getDurationMillisLeft(int userId) {
    struct itimerspec countdown;// = {0};
    if (timer_gettime(timers[userId], &countdown) < 0) {
        errorPrint("Could not get remaining time from timer for user %d!", userId);
        return -1;
    }
    return countdown.it_value.tv_sec * 1000 + countdown.it_value.tv_nsec / 10000000;
}

static void callTimerCallback(int sig, siginfo_t *si, void *uc) {
    int userId = si->si_value.sival_int;
    timerCallbacks[userId](userId);
}
