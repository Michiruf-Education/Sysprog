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

typedef struct user {
    int index;
    char username[USERNAMELENGTH]; //sicherstellen das der Username mit \0 Terminiert wird
    unsigned int score;
    int clientSocket; //Socket-Deskriptor
} USER;

void initUserData();

int addUser(char *username, int socketID);

void removeUser(int socketID);

int getUserAmount();

void clearUserData();

int nameExist(char *username);

void printUSERDATA();

void updateRankingAndSendPlayerList();

void updateRanking();


#endif
