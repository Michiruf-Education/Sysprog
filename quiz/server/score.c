/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * score.c: Implementierung des Score-Agents
 *
 * Implementieren Sie in diesem Modul den Score-Agent. Vermeiden Sie
 * Code-Duplikation, indem Sie auch hier Funktionen aus den Modulen user
 * und rfc wiederverwenden.
 * Achten Sie in diesem Modul besonders darauf, den Semaphor zum Triggern
 * des Score-Agents sauber wegzukapseln. Der Semaphor darf nur modul- und
 * nicht programmglobal sein.
 */

#include <semaphore.h>
#include "score.h"
#include "user.h"
#include "../common/util.h"
#include <pthread.h>
#include "rfc.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

static sem_t trigger;        // Zugriff nur über Funktionen dieses Moduls!
pthread_t scoreThreadID = 0;

//initialize Semaphore
int initSemaphore() {
    return sem_init(&trigger, 0, 0);
}

//increments (unlocks) Semaphore
int incrementScoreAgentSemaphore() {
    return sem_post(&trigger);
}

//Main - start function for the ScoreAgentThread
int startScoreAgentThread() {

    if (initSemaphore() >= 0) {
        int err;
        err = pthread_create(&scoreThreadID, NULL, (void *) &startScoreAgent, NULL);
        if (err == 0) {
            infoPrint("ScoreAgent thread created successfully");

            //waits for the thread until terminate
            pthread_join(scoreThreadID, NULL);
            return 1;
        } else {
            errorPrint("Error: Can't create Score agent thread");
            return err;
        }

    } else {
        errorPrint("Error: Semaphore could not be created/initialized");
        return -1;
    }
}

void startScoreAgent() {
    infoPrint("Starting ScoreAgent...");

    while (1) {

        //Waits until semaphor is incremented/unlocked and decrements (locks) it again
        sem_wait(&trigger);
        updateRanking();
        //SendPlayerListMSG

        //Create PlayerList
        PLAYER_LIST player_list = getPlayerList();
        MESSAGE sendmessage = buildPlayerList(player_list.players, getUserAmount());
        //fuer alle aktiven clients
        for (int i = 0; i < getUserAmount(); i++) {
            if (sendMessage(getSocketID(player_list.players[i].id), &sendmessage) >= 0) {
                debugPrint("Debug: ScoreAgent - PlayerList send");
            } else {
                errorPrint("Error: ScoreAgent Send Message PlayerList");
            }
        }
    }


}


