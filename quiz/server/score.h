/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * score.h: Header für den Score Agent
 */

#ifndef SCORE_H
#define SCORE_H

int startAwaitScoreAgentThread();

void startScoreAgent();

int initSemaphore();

int incrementScoreAgentSemaphore();

#endif
