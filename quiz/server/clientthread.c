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
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../common/util.h"
#include "clientthread.h"
#include "rfc.h"
#include "user.h"
#include "score.h"
#include "catalog.h"
#include "threadholder.h"
#include "login.h"
#include "rfchelper.h"
#include "usertimer.h"
#include "../common/question.h"

//------------------------------------------------------------------------------
// Method pre-declaration
//------------------------------------------------------------------------------
void clientThread(int *userIdPrt);

static int isMessageTypeAllowedInCurrentGameState(int gameState, int messageType);

static int isUserAuthorizedForMessageType(int messageType, int userId);

static void checkAndHandleAllPlayersFinished();

static void handleConnectionTimeout(int userId);

static void handleCatalogRequest(int userId);

static void handleCatalogChange(MESSAGE *message);

static void handleStartGame(MESSAGE *message, int userId);

static void handleQuestionRequest(int userId);

static void handleQuestionAnswered(MESSAGE *message, int userId);

//------------------------------------------------------------------------------
// Fields
//------------------------------------------------------------------------------
static int currentGameState;

static pthread_t clientThreadId = 0;

static char *selectedCatalogName = NULL;
static pthread_mutex_t selectedCatalogNameMutex;

static int currentQuestion[MAXUSERS] = {0};

static int finishedPlayerCount = 0;

//------------------------------------------------------------------------------
// Implementations
//------------------------------------------------------------------------------
int startClientThread(int userId) {
    // Initialize mutexes
    int mutexResult = pthread_mutex_init(&selectedCatalogNameMutex, NULL);
    if (mutexResult < 0) {
        errorPrint("Could not init selected catalog name MUTEX!");
        return mutexResult;
    }

    // Create thread
    int err = pthread_create(&clientThreadId, NULL, (void *) &clientThread, &userId);
    registerThread(clientThreadId);
    if (err == 0) {
        infoPrint("Client thread created successfully.");
    } else {
        errorPrint("Can't create client thread!");
    }
    return err;
}

void clientThread(int *userIdPtr) { // TODO FEEDBACK void pointers!
    int userId = *userIdPtr;

    if (isGameLeader(userId) >= 0) {
        currentGameState = GAME_STATE_PREPARATION;
    }

    while (1) {
        MESSAGE message;
        ssize_t messageSize = receiveMessage(getUser(userId).clientSocket, &message);
        if (messageSize > 0 && currentGameState != GAME_STATE_ABORTED) {
            if (validateMessage(&message) >= 0) {
                if (isMessageTypeAllowedInCurrentGameState(currentGameState, message.header.type) < 0) {
                    errorPrint("User %d not allowed to send RFC type %d in current game state: %d!", userId,
                               message.header.type, currentGameState);
                    return;
                    // TODO continue instead of return?
                }

                if (isUserAuthorizedForMessageType(userId, message.header.type) < 0) {
                    errorPrint("User %d not allowed to send RFC type %d!", userId, message.header.type);
                    return;
                    // TODO continue instead of return?
                }

                switch (message.header.type) {
                    case TYPE_CATALOG_REQUEST:
                        handleCatalogRequest(userId);
                        break;
                    case TYPE_CATALOG_CHANGE:
                        handleCatalogChange(&message);
                        break;
                    case TYPE_START_GAME:
                        handleStartGame(&message, userId);
                        break;
                    case TYPE_QUESTION_REQUEST:
                        handleQuestionRequest(userId);
                        break;
                    case TYPE_QUESTION_ANSWERED:
                        handleQuestionAnswered(&message, userId);
                        break;
                    default:
                        // Do nothing
                        break;
                }
            } else {
                errorPrint("Invalid RFC message!");
            }
        } else if (messageSize == 0 || currentGameState == GAME_STATE_ABORTED) {
            handleConnectionTimeout(userId);
            return; // Safe call to terminate loop
        } else {
            errorPrint("Error receiving message (message size: %zu)!", messageSize);
        }
    }
}

static int isMessageTypeAllowedInCurrentGameState(int gameState, int messageType) {
    return (gameState == GAME_STATE_PREPARATION && !(messageType > 0 && messageType <= 7)) ||
           (gameState == GAME_STATE_GAME_RUNNING && !(messageType > 7 && messageType <= 12)) ||
           (gameState == GAME_STATE_FINISHED) ||
           (gameState == GAME_STATE_ABORTED)
           ? -1 : 1;
}

static int isUserAuthorizedForMessageType(int messageType, int userId) {
    return isGameLeader(userId) >= 0 || !(messageType == TYPE_CATALOG_CHANGE || messageType == TYPE_START_GAME)
           ? 1 : -1;
}

static void checkAndHandleAllPlayersFinished() {
    if (finishedPlayerCount == getUserAmount()) {
        currentGameState = GAME_STATE_FINISHED;

        infoPrint("Game over!");
        lockUserData();
        for (int i = 0; i < getUserAmount(); i++) {
            USER user = getUserByIndex(i);
            MESSAGE gameOver = buildGameOver(
                    (uint8_t) getAndCalculateRankByUserId(user.id),
                    (uint32_t) user.score);
            if (sendMessage(user.clientSocket, &gameOver) < 0) {
                errorPrint("Unable to send game over to %s (%d)",
                           user.username,
                           user.id);
            }
        }
        unlockUserData();

        // Cancel main thread so the server gets shut down
        cancelMainThread();
    }
}

static void handleConnectionTimeout(int userId) {
    if (currentGameState != GAME_STATE_FINISHED) {
        errorPrint("Player %d has left the game!", userId);
    }

    if (isGameLeader(userId) >= 0 && currentGameState == GAME_STATE_PREPARATION) {
        MESSAGE errorWarning = buildErrorWarning(ERROR_WARNING_TYPE_FATAL, "Game leader has left the game.");
        broadcastMessageExcludeOneUser(&errorWarning, "Unable to send error warning to %s (%d)!", userId, 1);

        currentGameState = GAME_STATE_ABORTED;
    } else if (getUserAmount() - 1 < MINUSERS && currentGameState == GAME_STATE_GAME_RUNNING) {
        char *errorTextPlain = "Game cancelled because there are less than %d players left.";
        char *errorText = malloc(sizeof(errorTextPlain) + sizeof(MINUSERS));
        sprintf(errorText, errorTextPlain, MINUSERS);

        MESSAGE errorWarning = buildErrorWarning(ERROR_WARNING_TYPE_FATAL, errorText);
        broadcastMessageExcludeOneUser(&errorWarning, "Unable to send error warning to %s (%d)!", userId, 1);

        currentGameState = GAME_STATE_ABORTED;
    }

    // Just to be safe call the close of the socket (if it is not closed yet)
    infoPrint("Closing socket for user %d...", userId);
    close(getUser(userId).clientSocket);

    infoPrint("Removing user data for user %d...", userId);
    removeUserOverID(userId);

    infoPrint("Exiting client thread for user %d...", userId);
    unregisterThread(pthread_self());
    pthread_exit(0);
}

static void handleCatalogRequest(int userId) {
    for (int i = 0; i < getCatalogCount(); i++) {
        MESSAGE catalogResponse = buildCatalogResponse(getCatalogNameByIndex(i));
        if (sendMessage(getUser(userId).clientSocket, &catalogResponse) < 0) {
            errorPrint("Unable to send catalog response to %s (%d)!",
                       getUser(userId).username,
                       getUser(userId).id);
        }

        // We need to send a catalog change after the catalog request for new user to get the
        // selected catalog immediately and not have to wait for a catalog change
        // by the game leader
        pthread_mutex_lock(&selectedCatalogNameMutex);
        if (selectedCatalogName != NULL && strlen(selectedCatalogName) > 0) {
            MESSAGE catalogChange = buildCatalogChange(selectedCatalogName);
            if (sendMessage(getUser(userId).clientSocket, &catalogChange) < 0) {
                errorPrint("Unable to send catalog change (after catalog response) to %s (%d)!",
                           getUser(userId).username,
                           getUser(userId).id);
            }
        }
        pthread_mutex_unlock(&selectedCatalogNameMutex);
    }
}

static void handleCatalogChange(MESSAGE *message) {
    pthread_mutex_lock(&selectedCatalogNameMutex);
    // NOTE FEEDBACK We should use memcpy, but this crashes
    //memcpy(selectedCatalogName, message.body.catalogChange.fileName, strlen(message.body.catalogChange.fileName));
    selectedCatalogName = message->body.catalogChange.fileName;
    pthread_mutex_unlock(&selectedCatalogNameMutex);

    MESSAGE catalogChangeResponse = buildCatalogChange(message->body.catalogChange.fileName);
    broadcastMessage(&catalogChangeResponse, "Unable to send catalog change response to user %s (%d)!");
}

static void handleStartGame(MESSAGE *message, int userId) {
    lockUserData();

    if (getUserAmount() < MINUSERS) {
        MESSAGE errorWarning = buildErrorWarning(ERROR_WARNING_TYPE_WARNING,
                                                 "Cannot start game because there are too few participants!");
        if (sendMessage(getUser(userId).clientSocket, &errorWarning) < 0) {
            errorPrint("Unable to send error warning to %s (%d)!",
                       getUser(userId).username,
                       getUser(userId).id);
        }
        unlockUserData();
        return;
    }

    disableLogin();
    loadCatalog(message->body.startGame.catalog);
    currentGameState = GAME_STATE_GAME_RUNNING;

    MESSAGE startGameResponse = buildStartGame(message->body.startGame.catalog);
    broadcastMessageWithoutLock(&startGameResponse, "Unable to send start game response to user %s (%d)!");

    unlockUserData();
    notifyScoreAgent();
}

static void handleQuestionRequest(int userId) {
    MESSAGE questionResponse;
    if (currentQuestion[userId] < getLoadedQuestionCount()) {
        Question question = getLoadedQuestions()[currentQuestion[userId]];
        questionResponse = buildQuestion(question.question, question.answers, question.timeout);
    } else {
        questionResponse = buildQuestionEmpty();
        finishedPlayerCount++;
        checkAndHandleAllPlayersFinished(); // TODO should we do this also at disconnect? -> yes we should!
    }

    // Project description tells to start the timer before we send the question
    startTimer(userId);

    if (sendMessage(getUser(userId).clientSocket, &questionResponse) < 0) {
        errorPrint("Unable to send question to %s (%d)!",
                   getUser(userId).username,
                   getUser(userId).id);
    }
}

static void handleQuestionAnswered(MESSAGE *message, int userId) {
    if (currentQuestion[userId] >= getLoadedQuestionCount()) {
        errorPrint("%s (%d) requested question out of bounds (#%d out of %d). !",
                   getUser(userId).username,
                   getUser(userId).id,
                   currentQuestion[userId],
                   getLoadedQuestionCount());
        return;
    }

    Question question = getLoadedQuestions()[currentQuestion[userId]];
    long timeout = (long) question.timeout * 1000; // Convert to milliseconds
    long durationMillis = getCurrentTimerDurationMillis(userId);
    int inTime = durationMillis <= timeout;

    debugPrint("-- Answer -- timeout:\t%li", timeout);
    debugPrint("-- Answer -- duration:\t%li", durationMillis);
    debugPrint("-- Answer -- inTime:\t%s", inTime ? "yes" : "no");

    // Calculate points if answer is correct
    if (message->body.questionAnswered.selected == question.correct && inTime) {
        calcScoreForUserByID(timeout, durationMillis, userId);
        notifyScoreAgent();
    }

    // Send question result to client
    MESSAGE questionResult = buildQuestionResult(question.correct, inTime);
    if (sendMessage(getUser(userId).clientSocket, &questionResult) < 0) {
        errorPrint("Unable to send question result to %s (%d)!",
                   getUser(userId).username,
                   getUser(userId).id);
    }

    // Go to next question
    currentQuestion[userId]++;
}
