/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * rfc.h: Definitionen für das Netzwerkprotokoll gemäß dem RFC
 *
 * Zusätzlich zu den Deklarationen der Sende- und Empfangsfunktionen
 * beinhaltet dieser Header die Definitionen der über das Netzwerk
 * versendeten RFC-Strukturen.
 */
#ifndef RFC_H
#define RFC_H

#include <stdbool.h>
#include "../common/question.h"

#define RFC_VERSION 9
#define RFC_MAX_MESSAGE_SIZE 255
#define RFC_PLAYER_NAME_LENGTH 32

//------------------------------------------------------------------------------
// Type definition and parsing (switching)
//------------------------------------------------------------------------------

enum {
    TYPE_LOGIN_REQUEST = 1,
    TYPE_LOGIN_RESPONSE_SUCCESSFUL = 2,
    TYPE_CATALOG_REQUEST = 3,
    TYPE_CATALOG_RESPONSE = 4,
    TYPE_CATALOG_CHANGE = 5,
    TYPE_PLAYER_LIST = 6,
    TYPE_START_GAME = 7,
    TYPE_QUESTION_REQUEST = 8,
    TYPE_QUESTION = 9,
    TYPE_QUESTION_ANSWERED = 10,
    TYPE_QUESTION_RESULT = 11,
    TYPE_GAME_OVER = 12,
    TYPE_ERROR_WARNING = 255
};


//------------------------------------------------------------------------------
// Request types (from client)
//------------------------------------------------------------------------------

#pragma pack(1)
typedef struct {
    uint8_t type;
    uint16_t length;
} HEADER;

typedef struct {
    char name[RFC_PLAYER_NAME_LENGTH];
    uint8_t rfcVersion;
} LOGIN_REQUEST;

typedef struct {
} CATALOG_REQUEST;

typedef struct {
    char fileName[RFC_MAX_MESSAGE_SIZE];
} CATALOG_CHANGE;

typedef struct {
    /**
     * Catalog optional in server to client responses (because its known already).
     */
    char catalogs[];
} START_GAME;

typedef union {
    LOGIN_REQUEST loginRequest;
    //CATALOG_REQUEST catalogRequest; // empty
    CATALOG_CHANGE catalogChange;
    START_GAME startGame;
    // TODO QUESTION_REQUEST questionRequest;
    // TODO QUESTION_ANSWERED questionAnswered;
} BODY;

typedef struct {
    HEADER header;
    BODY body;
} MESSAGE;
#pragma pack(0)


//------------------------------------------------------------------------------
// Response/send types (to send to client)
//------------------------------------------------------------------------------

typedef struct player {
    char name[];
    int points;
    int id;
} PLAYER;


//------------------------------------------------------------------------------
// Methods for parsing and sending
//------------------------------------------------------------------------------

ssize_t receiveMessage(int socketId, MESSAGE *message);

int validateMessage(MESSAGE *message);

ssize_t sendMessage(int socketId, MESSAGE *message);

void sendLoginResponseSuccessful(int clientSocket, uint8_t rfcVersion, u_int8_t maxPlayerCount, u_int8_t clientId);
void buildLoginResponseSuccessfulMessage(uint8_t rfcVersion, u_int8_t maxPlayerCount, u_int8_t clientId); // TODO

void sendCatalogResponse(int clientSocket, /* nullable */ char catalogFileName[]);

void sendCatalogChange(int clientSocket, MESSAGE catalogChange);

void sendPlayerList(int clientSocket, PLAYER players[]);

void sendStartGame(int clientSocket, MESSAGE startGame);

/*
TODO Do this for next assignment
void sendQuestion(int clientSocket, ...);

void sendQuestionResult(int clientSocket, ...);

void sendGameOver(int clientSocket, ...);
*/

void sendErrorWarning(int clientSocket, ...);

#endif



/*
MESSAGE message;
receiveMessage(socketId, &message);
switch(message.header.type) {
    case TYPE_LOGIN_REQUEST:
        // message.body.loginRequest.... // TODO access the fields of the message here
        sendLoginResponseSuccessful(socketId, message, maxPlayerCount, clientId);
        break;
}
*/
