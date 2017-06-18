/**
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * usertimer.h: Header zur Verwaltung von Timern für Benutzer
 */
#ifndef USERTIMER_H
#define USERTIMER_H

void startTimer(int userId);

long getCurrentTimerDurationMillis(int userId);

#endif
