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
#include "threadholder.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

void startScoreAgent();

static pthread_t scoreThreadId = 0;
static sem_t scoreAgentTrigger;

int initSemaphore() {
    return sem_init(&scoreAgentTrigger, 0, 0);
}

int incrementScoreAgentSemaphore() {
    return sem_post(&scoreAgentTrigger);
}

int startScoreAgentThread() {
    int result;

    result = initSemaphore();
    if (result < 0) {
        errorPrint("Error: Semaphore could not be created/initialized");
        return result;
    }

    result = pthread_create(&scoreThreadId, NULL, (void *) &startScoreAgent, NULL);
    if (result != 0) {
        errorPrint("Error: Can't create Score agent thread");
        return -1;
    }

    registerThread(scoreThreadId);
    infoPrint("ScoreAgent thread created successfully");
    return 0;
}

void startScoreAgent() {
    infoPrint("Starting ScoreAgent...");

    while (1) {

        //Waits until semaphor is incremented/unlocked and decrements (locks) it again
        sem_wait(&scoreAgentTrigger);
        updateRanking();
        //SendPlayerListMSG

        //Create PlayerList
        // TODO We need a mutex here, because getPlayerList may return other data than the LATER call to getUserAmount
        // TODO Or we could not use shadow copies, but pointers (and mutex -> think of "leave case")                                
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


