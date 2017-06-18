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

int startTimer(int userId, int durationSeconds, void (*timerCallback)(int));

int stopTimer(int userId);

long getDurationMillisLeft(int userId);

#endif
