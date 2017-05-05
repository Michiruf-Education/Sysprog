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

static USER userdata[4];
static unsigned int userAmount = 0; //Aktuelle anzahl angemeldeter User

//reset/loescht inhalt der Zeile
void clearUserRow(int id) {
    pthread_mutex_lock(&mutexUserData);

    userdata[id].index = -1;
    userdata[id].username[0] = '\0';
    userdata[id].score = 0;
    userdata[id].clientSocket = -1;
    userAmount--;

    pthread_mutex_unlock(&mutexUserData);
}

//setzt user.index=-1
void clearUserData() {
    for (int i = 0; i < MAXUSERS; i++) {
        clearUserRow(i);
        userAmount = 0;
    }
}

//Error: Client ID no available for user beim zweiten durchlauf
int getClientIDforUser(int clientSocketID) {
    int clientID = -1;
    pthread_mutex_lock(&mutexUserData);
    for (int i = 0; i < MAXUSERS; i++) {
        if (userdata[i].clientSocket == clientSocketID) {
            pthread_mutex_unlock(&mutexUserData);
            clientID = i;
            i = MAXUSERS;
        } else {
            errorPrint("Error: Client ID no available for user");
            pthread_mutex_unlock(&mutexUserData);
            clientID = -1;
        }
    }

    pthread_mutex_unlock(&mutexUserData);
    return clientID;
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

//TODO test-debug code-reverse
PLAYER_LIST getPlayerList() {
    pthread_mutex_lock(&mutexUserData);
    PLAYER_LIST allActivePlayers;

    if (getUserAmount() > 0) {
        int nextSlot = 0;
        for (int i = 0; i < getUserAmount(); i++) {

            for (int j = nextSlot; j < MAXUSERS; j++) {
                PLAYER activePlayer;

                if (userdata[j].index != -1) {
                    memcpy(activePlayer.name, userdata[j].username, USERNAMELENGTH);
                    activePlayer.score = userdata[j].score;
                    activePlayer.id = userdata[j].index;

                    allActivePlayers.players[i] = activePlayer;
                    nextSlot = j;
                    j = MAXUSERS;
                }
                nextSlot++;
            }
        }
    }
    pthread_mutex_unlock(&mutexUserData);

    return allActivePlayers;
}

//gibt den Index des freien Speicherplatzes in userdata
//Bei fehler -1
int getFreeSlotID() {
    for (int i = 0; i < MAXUSERS; i++) {
        if (userdata[i].index == -1) {
            return i;
        }
    }
    return -1;
}

//Hinzufuegen eines Users
//Bei Fehler => -1
int addUser(char *username, int socketID) {
    //putchar('\n');

    //Mutex Lock
    pthread_mutex_lock(&mutexUserData);

    if (nameExist(username) == 0) {

        if (getUserAmount() < MAXUSERS) {

            int freeSlot = getFreeSlotID();
            if (freeSlot >= 0) {
                userdata[freeSlot].index = freeSlot;
                strcpy(userdata[freeSlot].username, username); //TODO prüfen !laeger als 32
                userdata[freeSlot].clientSocket = socketID;
                userdata[freeSlot].score = 0;

                userAmount++;

                pthread_mutex_unlock(&mutexUserData);
                incrementScoreAgentSemaphore(); //for ScoreAgent to be executed

                return 1;

            } else {
                pthread_mutex_unlock(&mutexUserData);
                errorPrint("Error: No free slot");
                return -1;
            }
        } else {
            errorPrint("Error: Maximum numbers of User reached, adding Username: %s not possible!", username);
            pthread_mutex_unlock(&mutexUserData);
            return -1;
        }

    } else {
        errorPrint("Error: User with Username: %s already exist!", username);
        pthread_mutex_unlock(&mutexUserData);
        return -1;
    }
}

USER getUser(int id) {
    return userdata[id];
}

USER getUserByIndex(int index) {
    // Implementation may be irritating, but we should implement this as we are able to
    // separate the index and the id in the future.
    // Doing so we could handle the disconnect of the leader (id: 0) to still get the first connected
    // user by index.
    for (int i = 0; i < MAXUSERS; i++) { // TODO
        if (getUser(i).index == -1) {
            index++;
        }
        if(i >= index) {
            break;
        }
    }
    return getUser(index);
}

int getSocketID(int id) {
    return userdata[id].clientSocket;
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
            incrementScoreAgentSemaphore(); //for ScoreAgent to be executed
        }
    }
}

//loescht ein User anhand des index/clientID
void removeUserOverID(int id) {
    clearUserRow(id);
    incrementScoreAgentSemaphore(); //for ScoreAgent to be executed
}


//TODO updateRanking
void updateRanking() {
    pthread_mutex_lock(&mutexUserData);
    //update Playerliste erstellen, ggf. Rangliste neu berechnen
    pthread_mutex_unlock(&mutexUserData);
}

//0 => ja
//-1=> nein
int isGameLeader(int id) {
    if (id == 0) {
        return 0;
    } else {
        return -1;
    }

}

//DEBUG print UserData
void printUSERDATA() {
    infoPrint("\n\n");
    infoPrint("/---------------------------------------------------------------\\\n");
    for (int i = 0; i < MAXUSERS; i++) {
        infoPrint("| ID: %d\t| Username: %s\t| score: %d\t| SocketID:%d\t|\n", userdata[i].index, userdata[i].username,
                  userdata[i].score, userdata[i].clientSocket);
    }
    infoPrint("\\---------------------------------------------------------------/ \n");
}

void printPLAYERLIST() {
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

