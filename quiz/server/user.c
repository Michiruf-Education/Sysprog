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

int getClientIDforUser(int clientSocketID) {
    pthread_mutex_lock(&mutexUserData);

    for (int i = 0; i < MAXUSERS; i++) {
        if (userdata[i].clientSocket == clientSocketID) {
            pthread_mutex_unlock(&mutexUserData);
            return i;

        } else {
            errorPrint("Error: Client ID no available for user");
            pthread_mutex_unlock(&mutexUserData);
            return -1;
        }
    }

    pthread_mutex_unlock(&mutexUserData);
    return -1;
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
                incrementSemaphore(); //for ScoreAgent to be executed

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

USER getUser(int id){
    return userdata[id];
}

USER getUserByIndex(int index) {
    // Implementation may be irritating, but we should implement this as we are able to
    // separate the index and the id in the future.
    // Doing so we could handle the disconnect of the leader (id: 0) to still get the first connected
    // user by index.
    return getUser(index);
}

int getSocketID(int id){
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
            incrementSemaphore(); //for ScoreAgent to be executed
        }
    }
}

//loescht ein User anhand des index/clientID
void removeUserOverID(int id) {
    clearUserRow(id);
    incrementSemaphore(); //for ScoreAgent to be executed
}


//TODO updateRanking
void updateRanking() {
    pthread_mutex_lock(&mutexUserData);
    //update Playerliste erstellen, ggf. Rangliste neu berechnen
    pthread_mutex_unlock(&mutexUserData);
}

//TODO Fuer Aufgabe 4 aktuell nur aktuellen Spielteilnehmer versenden, Rangliste wird noch nicht erstellt
void updateRankingSendPlayerList() {

    updateRanking();
    //TODO versenden der Playerliste
    //LST - Nachricht senden
    //rfc-functionen verwenden


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
    printf("\n\n");
    printf("/---------------------------------------------------------------\\\n");
    for (int i = 0; i < MAXUSERS; i++) {
        printf("| ID: %d\t| Username: %s\t| score: %d\t| SocketID:%d\t|\n", userdata[i].index, userdata[i].username,
               userdata[i].score, userdata[i].clientSocket);
    }
    printf("\\---------------------------------------------------------------/ \n");
}


