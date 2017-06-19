/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * user.h: Header für die User-Verwaltung
 */

#ifndef USER_H
#define USER_H

#include "vardefine.h"
#include "rfc.h"

typedef struct user {
    int id;
    char username[USERNAMELENGTH]; //sicherstellen das der Username mit \0 Terminiert wird
    unsigned int score;
    int clientSocket; //Socket-Deskriptor
} USER;

int initUserData();

int addUser(char *username, int socketID);

void removeUserOverSocketID(int socketID);

void removeUser(int userId);

USER getUser(int userId);

USER getUserByIndex(int index);

int getSocketIdByUserId(int userId);

int getUserAmount();

int getUserIdByClientSocket(int clientSocket);

void clearUserData();

int nameExist(char *username);

int isGameLeader(int userId);

PLAYER_LIST getPlayerList();

PLAYER_LIST getPlayerListSortedByScore();

void lockUserData();

void unlockUserData();

//Calc score for the user given, question timeout, needed time to answer, and clientSocket
void calcScoreForUserByID(long timeout, long neededtime, int id);

int getAndCalculateRankByUserId(int userId);

//Debug functions
void printUSERDATA();

void printPlayerList();

void printPlayerListSortedByScore();

#endif
