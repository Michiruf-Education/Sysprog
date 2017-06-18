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

//TODO parameter umbennen/anpassen

typedef struct user {
    int id;
    char username[USERNAMELENGTH]; //sicherstellen das der Username mit \0 Terminiert wird
    unsigned int score;
    int clientSocket; //Socket-Deskriptor
} USER;

int initUserData();

int addUser(char *username, int socketID);

void removeUserOverSocketID(int socketID);

void removeUserOverID(int id);

USER getUser(int id);

USER getUserByIndex(int id);

int getSocketID(int id);

int getUserAmount();

int getUserIdByClientSocket(int clientSocket);

void clearUserData();

int nameExist(char *username);

int isGameLeader(int id);

PLAYER_LIST getPlayerList();

PLAYER_LIST getPlayerListSortedByScore();

void printUSERDATA();

void printPlayerList();

void printPlayerListSortedByScore();

void lockUserData();

void unlockUserData();

//Calc score for the user given, question timeout, needed time to answer, and clientSocket
void calcScoreForUserByID(int timeout, int neededtime, int id);

int getAndCalculateRankByUserId(int userId);

#endif
