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

void createTimer(int userId);

void startTimer(int userId);

int getCurrentTimerDuration(int userId);

void stopTimer(int userId);

#endif
