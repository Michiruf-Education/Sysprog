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

//Main - start function for the ScoreAgentThread
int startScoreAgentThread();

//initialize Semaphore
int initSemaphore();

//update Ranking
void updateRanking();

//increments (unlocks) Semaphore
int incrementScoreAgentSemaphore();

#endif
