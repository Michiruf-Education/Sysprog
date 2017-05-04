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

static sem_t trigger;		// Zugriff nur über Funktionen dieses Moduls!
pthread_t scoreThreadID = 0;

//starts a ScoreAgentThread
int startScoreAgentThread(){
    int err;
    err=pthread_create(&scoreThreadID,NULL,(void * ) &startScoreAgent,NULL);
    if(err == 0){
        infoPrint("Score agent thread created successfully");
    }else{
        errorPrint("Can't create Score agent thread");
    }
    return err;
}

void startScoreAgent(){
    infoPrint("Starting ScoreAgent...");

    while(1){
        sem_wait(&trigger);

        //updateRankingSendPlayerList();


    }


}