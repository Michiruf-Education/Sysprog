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
#include <sys/types.h>
#include "../common/util.h"
#include "clientthread.h"
#include "rfc.h"
#include "user.h"
#include "score.h"

//------------------------------------------------------------------------------
// Fields and method pre-declaration
//------------------------------------------------------------------------------
int currentGameState;

pthread_t clientThreadId = 0;

void clientThread(int userId);

static void cleanupClientThread(int userId);

static int isMessageTypeAllowedInCurrentGameState(int gameState, int messageType);

static int isUserAuthorizedForMessageType(int messageType, int userId);

static void handleConnectionTimeout(int userId);

static void handleCatalogRequest(int userId);

static void handleCatalogChange(MESSAGE message);

static void handleStartGame(MESSAGE message, int userId);

static void handleQuestionRequest(MESSAGE message, int userId);

static void handleQuestionAnswered(MESSAGE message, int userId);

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
                if (isMessageTypeAllowedInCurrentGameState(currentGameState, message.header.type) < 0) {
                    errorPrint("User %d not allowed to send RFC type %d in current game state: %d!", userId,
                               message.header.type, currentGameState);
                    return;
                }

                if (isUserAuthorizedForMessageType(userId, message.header.type) < 0) {
                    errorPrint("User %d not allowed to send RFC type %d!", userId, message.header.type);
                    return;
                }

                switch (message.header.type) {
                    case TYPE_CATALOG_REQUEST:
                        handleCatalogRequest(userId);
                        break;
                    case TYPE_CATALOG_CHANGE:
                        handleCatalogChange(message);
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

static int isMessageTypeAllowedInCurrentGameState(int gameState, int messageType) {
    return (gameState == GAME_STATE_PREPARATION && !(messageType > 0 && messageType <= 7)) ||
           (gameState == GAME_STATE_GAME_RUNNING && !(messageType > 7 && messageType <= 12)) ||
           (gameState == GAME_STATE_FINISHED)
           ? -1 : 1;
}

static int isUserAuthorizedForMessageType(int messageType, int userId) {
    return isGameLeader(userId) >= 0 || !(messageType == TYPE_CATALOG_CHANGE || messageType == TYPE_START_GAME);
}

static void handleConnectionTimeout(int userId) {
    errorPrint("Player %d has left the game", userId);

    if (isGameLeader(userId) > 0 && currentGameState == GAME_STATE_PREPARATION) {
        MESSAGE errorWarning = buildErrorWarning(ERROR_WARNING_TYPE_FATAL, "Game leader left the game.");
        if (sendMessage(getUser(userId).clientSocket, &errorWarning) < 0) {
            errorPrint("Unable to send error warning to %s (%d)!",
                       getUser(userId).username,
                       getUser(userId).index);
        }
//        cancelAllServerThreads(); // TODO need to do a central station to register threads
        return;
    }

    if (getUserAmount() <= 2 && currentGameState == GAME_STATE_GAME_RUNNING) {
        MESSAGE errorWarning = buildErrorWarning(ERROR_WARNING_TYPE_FATAL,
                                                 "Game cancelled because there are less than 2 players left.");
        if (sendMessage(getUser(userId).clientSocket, &errorWarning) < 0) {
            errorPrint("Unable to send error warning to %s (%d)!",
                       getUser(userId).username,
                       getUser(userId).index);
        }
//        cancelAllServerThreads();
        return;
    }

    errorPrint("Connection timeout by %d", userId);
    pthread_exit(0); // TODO lookup man again!
}

static void handleCatalogRequest(int userId) {
    errorPrint("GOT CATALOG REQUEST!");
    // TODO
//    for (int i = 0; i < getCatalogCount(); i++) { // TODO shell contain the empty catalog!
//        MESSAGE catalogResponse = buildCatalogResponse(getCatalog(i).name);
//        if (sendMessage(getUser(userId).clientSocket, &catalogResponse) < 0) {
//            errorPrint("Unable to send catalog response to %s (%d)!",
//                       getUser(userId).username,
//                       getUser(userId).index);
//        }
//    }
}

static void handleCatalogChange(MESSAGE message) {
    MESSAGE catalogChangeResponse = buildCatalogChange(message.body.catalogChange.fileName);
    for (int i = 0; i < getUserAmount(); i++) {
        if (sendMessage(getUserByIndex(i).clientSocket, &catalogChangeResponse) < 0) {
            errorPrint("Unable to send catalog change response to user %s (%d)!",
                       getUserByIndex(i).username,
                       getUserByIndex(i).index);
        }
    }
}

static void handleStartGame(MESSAGE message, int userId) {
    if (getUserAmount() < 4) { // TODO remove hardcoded stuff -> min-players define
        MESSAGE errorWarning = buildErrorWarning(ERROR_WARNING_TYPE_WARNING,
                                                 "Cannot start game because there are too few participants!");
        if (sendMessage(getUser(userId).clientSocket, &errorWarning) < 0) {
            errorPrint("Unable to send error warning to %s (%d)!",
                       getUser(userId).username,
                       getUser(userId).index);
        }
        return;
    }

    // TODO
//    loadCatalog(message.body.startGame.catalog);
    currentGameState = GAME_STATE_GAME_RUNNING;

    MESSAGE startGameResponse = buildStartGame(message.body.startGame.catalog);
    for (int i = 0; i < getUserAmount(); i++) {
        if (sendMessage(getUserByIndex(i).clientSocket, &startGameResponse) < 0) {
            errorPrint("Unable to send start game response to user %s (%d)!",
                       getUserByIndex(i).username,
                       getUserByIndex(i).index);
        }
    }

    incrementScoreAgentSemaphore();
}

static void handleQuestionRequest(MESSAGE message, int userId) {
    // TODO Next assignment
}

static void handleQuestionAnswered(MESSAGE message, int userId) {
    // TODO Next assignment
}
