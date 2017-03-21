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

static sem_t trigger;		// Zugriff nur über Funktionen dieses Moduls!
