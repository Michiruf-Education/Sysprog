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

int startScoreAgentThread();

void startScoreAgent();

int initSemaphore();

int incrementScoreAgentSemaphore();

#endif
