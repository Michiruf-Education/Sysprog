/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * user.c: Implementierung der User-Verwaltung
 *
 * Die angemeldeten Clients werden in einem Array oder einer verketteten
 * Liste aus C-Strukturen verwaltet. Dieses Modul enthält die Funktionen
 * zum Verwalten dieser Daten. Dies beinhaltet das Eintragen und Löschen
 * von Clients und das Iterieren über die Einträge.
 * Da diese Datenstruktur von mehreren Threads gleichzeitig verwendet wird,
 * ist auf die korrekte Synchronisierung zu achten!
 */
#include <stdio.h>
#include <string.h>
#include "user.h"
#include <pthread.h>
#include <stdlib.h>
#include "../common/util.h"
#include "vardefine.h"
#include "score.h"
#include "rfc.h"

//pthread_t
pthread_mutex_t mutexUserData;

static USER userdata[MAXUSERS];
static unsigned int userAmount = 0; //Aktuelle anzahl angemeldeter User

//reset/loescht inhalt der Zeile
void clearUserRow(int id) {
    lockUserData();

    userdata[id].id = -1;
    userdata[id].username[0] = '\0';
    userdata[id].score = 0;
    userdata[id].clientSocket = -1;
    userAmount--;

    unlockUserData();
}

void lockUserData() {
    pthread_mutex_lock(&mutexUserData);
}

void unlockUserData() {
    pthread_mutex_unlock(&mutexUserData);
}

//setzt user.id=-1
void clearUserData() {
    for (int i = 0; i < MAXUSERS; i++) {
        clearUserRow(i);
        userAmount = 0;
    }
}

//Error: Client ID no available for user beim zweiten durchlauf
int getUserIdByClientSocket(int clientSocket) {
    lockUserData();
    int clientId = -1;

    for (int i = 0; i < MAXUSERS; i++) {
        if (userdata[i].clientSocket == clientSocket) {
            clientId = i;
            break;
        }
    }

    if (clientId == -1) {
        errorPrint("Error: Client ID not available for user");
    }

    unlockUserData();
    return clientId;
}

int initMutex() {
    return pthread_mutex_init(&mutexUserData, NULL);
}

//init UserData
int initUserData() {
    if (initMutex() >= 0) {
        clearUserData();
        return 1;
    } else {
        errorPrint("Error: Mutex could not be initialized");
        return -1;
    }
}

//gibt aktuelle anzahl der angemeldeten User zurück
int getUserAmount() {
    return userAmount;
}

//getPlayerList Sorted by Score
//TODO shitty implementaion but works so far
PLAYER_LIST getPlayerListSortedByScore() {
    PLAYER_LIST allActivePlayers = getPlayerList();
    PLAYER tmpforSwap;
    int userAmount = getUserAmount();

    //Use Bubble Sort for Sorting PlayerList
    if (userAmount > 0) {
        for (int i = 0; i < userAmount - 1; i++) {
            for (int j = 0; j < userAmount - 1; j++) {

                if (allActivePlayers.players[i].score < allActivePlayers.players[j + 1].score) {
                    //Swap
                    tmpforSwap = allActivePlayers.players[j];
                    allActivePlayers.players[j] = allActivePlayers.players[j + 1];
                    allActivePlayers.players[j + 1] = tmpforSwap;
                }
            }
        }

        for (int i = 0; i < userAmount - 1; i++) {
            for (int j = 0; j < userAmount - 1; j++) {

                if (allActivePlayers.players[i].score < allActivePlayers.players[j + 1].score) {
                    //Swap
                    tmpforSwap = allActivePlayers.players[j];
                    allActivePlayers.players[j] = allActivePlayers.players[j + 1];
                    allActivePlayers.players[j + 1] = tmpforSwap;
                }
            }
        }
    }

    //DEBUG TODO remove
    PLAYER_LIST tmpPlayerLst = allActivePlayers;

    infoPrint("\n\n");
    infoPrint("/----------------PLAYER-LIST-sorted by Score---------------------\\\n");
    for (
            int i = 0;
            i < getUserAmount();
            i++) {
        infoPrint("| ID: %d\t| Username: %s\t| score: %d\t|\n", tmpPlayerLst.players[i].id,
                  tmpPlayerLst.players[i].name,
                  tmpPlayerLst.players[i].score);
    }
    infoPrint("\\---------------------------------------------------------------/ \n");


    return allActivePlayers;
}

//Returns Rank of user 1-4
//TODO bei gleicher Punktzahl selben platz zurueck geben
int getAndCalculateRankByUserId(int userId) {
    int rank = -1;
    PLAYER_LIST sortedPlayerList = getPlayerListSortedByScore();
    if (getUserAmount() > 0) {
        for (int i = 0; i < getUserAmount(); i++) {
            if (sortedPlayerList.players[i].id == userId) {
                rank = i + 1;
                break;
            }
        }
    }
    return rank;
}


PLAYER_LIST getPlayerList() {
    PLAYER_LIST allActivePlayers;

    if (getUserAmount() > 0) {
        int nextSlot = 0;
        for (int i = 0; i < getUserAmount(); i++) {

            for (int j = nextSlot; j < MAXUSERS; j++) {
                PLAYER activePlayer;

                if (userdata[j].id != -1) {
                    memcpy(activePlayer.name, userdata[j].username, USERNAMELENGTH);
                    activePlayer.score = userdata[j].score;
                    activePlayer.id = userdata[j].id;

                    allActivePlayers.players[i] = activePlayer;
                    nextSlot = j;
                    j = MAXUSERS;
                }
                nextSlot++;
            }
        }
    }

    return allActivePlayers;
}

//gibt den Index des freien Speicherplatzes in userdata
//Bei fehler -1
static int getFreeSlotID() {
    for (int i = 0; i < MAXUSERS; i++) {
        if (userdata[i].id == -1) {
            return i;
        }
    }
    return -1;
}

//Hinzufuegen eines Users
//Bei Fehler => -1
int addUser(char *username, int socketID) {
    lockUserData();

    if (strlen(username) > USERNAMELENGTH) {
        errorPrint("Username to long!");
        return -1;
    }

    if (nameExist(username) != 0) {
        errorPrint("Error: User with Username: %s already exist!", username);

        MESSAGE errorWarning = buildErrorWarning(ERROR_WARNING_TYPE_FATAL, "User with Username already exist!");
        if (sendMessage(socketID, &errorWarning) < 0) {
            errorPrint("Unable to send error warning to");
        }

        unlockUserData();
        return -2;
    }

    if (getUserAmount() >= MAXUSERS) {
        errorPrint("Error: Maximum numbers of User reached, adding Username: %s not possible!", username);

        MESSAGE errorWarning = buildErrorWarning(ERROR_WARNING_TYPE_FATAL,
                                                 "Maximum numbers of User reached, adding Username not possible!");
        if (sendMessage(socketID, &errorWarning) < 0) {
            errorPrint("Unable to send error warning to");
        }

        unlockUserData();
        return -3;
    }

    int freeSlot = getFreeSlotID();
    if (freeSlot < 0) {
        errorPrint("Error: No free slot");
        unlockUserData();
        return -4;
    }

    userdata[freeSlot].id = freeSlot;
    strcpy(userdata[freeSlot].username, username);
    userdata[freeSlot].clientSocket = socketID;
    userdata[freeSlot].score = 0;
    userAmount++;

    unlockUserData();
    notifyScoreAgent(); //for ScoreAgent to be executed

    return 1;
}

USER getUser(int userId) {
    return userdata[userId];
}

USER getUserByIndex(int index) {
    // Implementation may be irritating, but we should implement this as we are able to
    // separate the index and the id in the future.
    // Doing so we could handle the disconnect of the leader (id: 0) to still get the first connected
    // user by index.
    for (int i = 0; i < MAXUSERS; i++) {
        if (getUser(i).id == -1) {
            index++;
        }
        if (i >= index) {
            break;
        }
    }
    return getUser(index);
}

int getSocketIdByUserId(int userId) {
    return userdata[userId].clientSocket;
}

//return 1 => true
//return 0 => false
int nameExist(char *username) {

    for (int i = 0; i < MAXUSERS; i++) {
        if (strcmp(userdata[i].username, username) == 0) {
            return 1;
        }
    }

    return 0;
}

//loescht ein User anhand der socketID
void removeUserOverSocketID(int socketID) {
    for (int i = 0; i < MAXUSERS; i++) {
        if (userdata[i].clientSocket == socketID) {
            clearUserRow(i);
            notifyScoreAgent(); //for ScoreAgent to be executed
        }
    }
}

//loescht ein User anhand der ID
void removeUser(int userId) {
    clearUserRow(userId);
    notifyScoreAgent(); //for ScoreAgent to be executed
}

//0 => ja
//-1=> nein
int isGameLeader(int userId) {
    if (userId == 0) {
        return 0;
    } else {
        return -1;
    }

}

//Calc score
unsigned int scoreForTimeLeft(long timeout, long timeLeft) {
    unsigned int score = (timeLeft * 1000UL) / timeout; /* auf 10er-Stellen runden */
    score = ((score + 5UL) / 10UL) * 10UL;
    return score;
}

//Calc score and set for the user given, question timeout, needed time to answer, and clientSocket
void calcScoreForUserByID(long timeout, long neededtime, int id) {

    unsigned int scoreForCurrentQuestion = scoreForTimeLeft(timeout, (timeout - neededtime));

    lockUserData();
    for (int i = 0; i < MAXUSERS; i++) {
        if (userdata[i].id == id) {
            userdata[i].score += scoreForCurrentQuestion;
        }
    }
    unlockUserData();

}

//DEBUG print UserData
void printUSERDATA() {
    infoPrint("\n\n");
    infoPrint("/---------------------------------------------------------------\\\n");
    for (int i = 0; i < MAXUSERS; i++) {
        infoPrint("| ID: %d\t| Username: %s\t| score: %d\t| SocketID:%d\t|\n", userdata[i].id, userdata[i].username,
                  userdata[i].score, userdata[i].clientSocket);
    }
    infoPrint("\\---------------------------------------------------------------/ \n");
}

void printPlayerList() {
    PLAYER_LIST tmpPlayerLst = getPlayerList();

    infoPrint("\n\n");
    infoPrint("/------------------------PLAYER-LIST------------------------------\\\n");
    for (int i = 0; i < getUserAmount(); i++) {
        infoPrint("| ID: %d\t| Username: %s\t| score: %d\t|\n", tmpPlayerLst.players[i].id,
                  tmpPlayerLst.players[i].name,
                  tmpPlayerLst.players[i].score);
    }
    infoPrint("\\---------------------------------------------------------------/ \n");
}

void printPlayerListSortedByScore() {
    PLAYER_LIST tmpPlayerLst = getPlayerListSortedByScore();

    infoPrint("\n\n");
    infoPrint("/----------------PLAYER-LIST-sorted by Score---------------------\\\n");
    for (int i = 0; i < getUserAmount(); i++) {
        infoPrint("| ID: %d\t| Username: %s\t| score: %d\t|\n", tmpPlayerLst.players[i].id,
                  tmpPlayerLst.players[i].name,
                  tmpPlayerLst.players[i].score);
    }
    infoPrint("\\---------------------------------------------------------------/ \n");
}


