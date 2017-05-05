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

//TODO parameter umbennen/anpassen

typedef struct user {
    int index;
    char username[USERNAMELENGTH]; //sicherstellen das der Username mit \0 Terminiert wird
    unsigned int score;
    int clientSocket; //Socket-Deskriptor
} USER;

int initUserData();

int addUser(char *username, int socketID);

void removeUserOverSocketID(int socketID);

void removeUserOverID(int id);

USER getUser(int id);

USER getUserByIndex(int index);

int getSocketID(int id);

int getUserAmount();

int getClientIDforUser(int clientSocket);

void clearUserData();

int nameExist(char *username);

void printUSERDATA();

void updateRanking();

int isGameLeader(int id);


#endif
