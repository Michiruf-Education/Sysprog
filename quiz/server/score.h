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
int incrementScoreAgentSemaphore(); // TODO Rename notifyScoreAgent!

//Calc score for the user given, question timeout, needed time to answer, and clientSocket
void calcScoreForUserByID(int timeout, int neededtime, int id);
#endif
