/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * user.h: Header für die User-Verwaltung
 */

#include "vardefine.h"

#ifndef USER_H
#define USER_H



typedef struct user {
    int index;
    char username[USERNAMELENGTH]; //sicherstellen das der Username mit \0 Terminiert wird
    unsigned int score;
    int clientSocket; //Socket-Deskriptor
} USER;

void clearUserRow(int id);
void clearUserData();
void initUserData();
int getUserAmount();
int getFreeSlotID();
int addUser(char *username, int socketID);
int nameExist(char *username);
void removeUser(int socketID);
void printUSERDATA();
void updateRankingSendPlayerList();
void updateRanking();


#endif
