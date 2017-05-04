/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * clientthread.c: Implementierung des Client-Threads
 *
 * In diesem Modul wird die an phread_create(3) übergebene Threadfunktion für
 * den Client-Thread implementiert. Bitte nutzen Sie modulgebundene (static)
 * Hilfsfunktionen, um die Implementierung übersichtlich zu halten und
 * schreiben Sie nicht alles in eine einzige große Funktion.
 * Verwenden Sie zum Senden und Empfangen von Nachrichten die von Ihnen
 * definierten Funktionen und Strukturen aus dem RFC-Modul.
 * Benutzen Sie für den Zugriff auf die User-Liste das Modul user.
 * Zum Ansteuern des Loaders und zur Verwaltung der Fragekataloge sollen
 * die Funktionen aus dem Modul catalog verwendet werden.
 */
#include <pthread.h>
#include "../common/util.h"
#include "clientthread.h"
#include "rfc.h"
#include "user.h"

//------------------------------------------------------------------------------
// Fields and method predeclaration
//------------------------------------------------------------------------------
int currentGameState;

pthread_t clientThreadId = 0;

void clientThread(int userId);

void cleanupClientThread(int userId);

void handleConnectionTimeout(int userId);

void handleCatalogRequest(MESSAGE message, int userId);

void handleCatalogChange(MESSAGE message, int userId);

void handleStartGame(MESSAGE message, int userId);

void handleQuestionRequest(MESSAGE message, int userId);

void handleQuestionAnswered(MESSAGE message, int userId);

//------------------------------------------------------------------------------
// Implementations
//------------------------------------------------------------------------------
int initializeClientThread(int userId) {
    int err = pthread_create(&clientThreadId, NULL, (void *) &clientThread, &userId);
    if (err == 0) {
        infoPrint("Client thread created successfully.");
    } else {
        errorPrint("Can't create client thread!");
    }
    return err;
}

void clientThread(int userId) {
    //pthread_cleanup_push((void *) &cleanupClientThread, &userId);

    if (isGameLeader(userId)) {
        currentGameState = GAME_STATE_PREPARATION;
    }

    while (1) {
        MESSAGE message;
        ssize_t messageSize = receiveMessage(getUser(userId).clientSocket, &message) >= 0 && validateMessage(&message);
        if (messageSize > 0) {
            if (validateMessage(&message) >= 0) {
                if (isMessageTypeAllowedInCurrentGameState(message.header.type, userId) < 0) {
                    errorPrint("User %d not allowed to send RFC type %d in current game state!", userId,
                               message.header.type);
                    return;
                }

                if (isUserAuthorizedForMessageType(userId, userId, message.header.type) < 0) {
                    errorPrint("User %d not allowed to send RFC type %d!", userId, message.header.type);
                    return;
                }

                switch (message.header.type) {
                    case TYPE_CATALOG_REQUEST:
                        handleCatalogRequest(message, userId);
                        break;
                    case TYPE_CATALOG_CHANGE:
                        handleCatalogChange(message, userId);
                        break;
                    case TYPE_START_GAME:
                        handleStartGame(message, userId);
                        break;
                    case TYPE_QUESTION_REQUEST:
                        handleQuestionRequest(message, userId);
                        break;
                    case TYPE_QUESTION_ANSWERED:
                        handleQuestionAnswered(message, userId);
                        break;
                    default:
                        // Do nothing
                        break;
                }
            } else {
                errorPrint("Invalid RFC message!");
            }
        } else if (messageSize == 0) {
            handleConnectionTimeout(userId);
        } else {
            errorPrint("Error receiving message!");
        }
    }
}

static void cleanupClientThread(int userId) {
    // TODO
}

static void handleConnectionTimeout(int userId) {
    errorPrint("Player %d has left the game", userId);

    if (isGameLeader(userId) > 0 && currentGameState == GAME_STATE_PREPARATION) {
        sendGlobalError("Game leader left the game.");
        cancelAllServerThreads(); // TODO need to do a central station to register threads
    } else if (getUserAmount() <= 2 && currentGameState == GAME_STATE_GAME_RUNNING) {
        sendGlobalError("Game cancelled because there are less than 2 players left.");
        cancelAllServerThreads();
    } else {
        pthread_exit(0);
    }
}

static void handleCatalogRequest(MESSAGE message, int userId) {
    // TODO
}

static void handleCatalogChange(MESSAGE message, int userId) {
    // TODO
}

static void handleStartGame(MESSAGE message, int userId) {
    // TODO
}

static void handleQuestionRequest(MESSAGE message, int userId) {
    // TODO
}

static void handleQuestionAnswered(MESSAGE message, int userId) {
    // TODO
}
