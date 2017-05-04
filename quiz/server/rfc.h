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

#include "../common/question.h"

#define RFC_VERSION 9
#define RFC_MAX_MESSAGE_SIZE 255
#define RFC_PLAYER_NAME_LENGTH 32
#define RFC_PLAYER_COUNT_MAXIMUM 4
#define RFC_ERROR_WARNING_MAX_LENGTH 400

//------------------------------------------------------------------------------
// Type definition and parsing (for switching)
//------------------------------------------------------------------------------

enum {
    TYPE_LOGIN_REQUEST = 1,
    TYPE_LOGIN_RESPONSE_OK = 2,
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
// Request and response types
//------------------------------------------------------------------------------

#pragma pack(push, 1)
typedef struct {
    uint8_t rfcVersion;
    char name[RFC_PLAYER_NAME_LENGTH];
} LOGIN_REQUEST;

typedef struct {
    uint8_t rfcVersion;
    uint8_t maxPlayers;
    uint8_t clientId;
} LOGIN_RESPONSE_OK;

typedef struct {
    char fileName[RFC_MAX_MESSAGE_SIZE]; // optional
} CATALOG_RESPONSE;

typedef struct {
    char fileName[RFC_MAX_MESSAGE_SIZE];
} CATALOG_CHANGE;

typedef struct {
    char name[RFC_PLAYER_NAME_LENGTH];
    unsigned int score;
    uint8_t id;
} PLAYER;

typedef struct {
    PLAYER players[RFC_PLAYER_COUNT_MAXIMUM];
} PLAYER_LIST;

typedef struct {
    char catalog[RFC_MAX_MESSAGE_SIZE]; // optional in server to client responses
} START_GAME;

typedef struct {
    uint8_t subtype;
    char message[RFC_ERROR_WARNING_MAX_LENGTH];
} ERROR_WARNING;
#pragma pack(pop)


//------------------------------------------------------------------------------
// Message structure
//------------------------------------------------------------------------------

#pragma pack(push, 1)
typedef struct {
    uint8_t type;
    uint16_t length;
} HEADER;

typedef union {
    LOGIN_REQUEST loginRequest;
    LOGIN_RESPONSE_OK loginResponseOk;
    //CATALOG_REQUEST catalogRequest; // empty
    CATALOG_RESPONSE catalogResponse;
    CATALOG_CHANGE catalogChange;
    PLAYER_LIST playerList;
    START_GAME startGame;
    // TODO QUESTION_REQUEST questionRequest; --> use question.h
    // TODO QUESTION question;
    // TODO QUESTION_ANSWERED questionAnswered;
    // TODO QUESTION_RESULT questionResult;
    // TODO GAME_OVER gameOver;
    ERROR_WARNING errorWarning;
} BODY;

typedef struct {
    HEADER header;
    BODY body;
} MESSAGE;
#pragma pack(pop)


//------------------------------------------------------------------------------
// Methods for sending, receiving and building messages
//------------------------------------------------------------------------------

ssize_t receiveMessage(int socketId, MESSAGE *message);

int validateMessage(MESSAGE *message);

ssize_t sendMessage(int socketId, MESSAGE *message);

MESSAGE buildLoginResponseOk(uint8_t rfcVersion, uint8_t maxPlayerCount, uint8_t clientId);

MESSAGE buildCatalogResponse(/* nullable */ char catalogFileName[]);

MESSAGE buildCatalogChange(char catalogFileName[]);

MESSAGE buildPlayerList(PLAYER players[], int playerCount);

MESSAGE buildStartGame(/* nullable */ char catalogFileName[]);

/*
TODO Do this for next assignment
void buildQuestion(...);

void buildQuestionResult(...);

void buildGameOver(...);
*/

MESSAGE buildErrorWarning(uint8_t subtype, char message[]);

#endif



/*
MESSAGE message;
receiveMessage(socketId, &message);
switch(message.header.type) {
    case TYPE_LOGIN_REQUEST:
        // message.body.loginRequest.... // TODO access the fields of the message here
        MESSAGE msg = buildLoginResponseOk(message.body.loginRequest.rfcVersion, maxPlayerCount, clientId);
        sendMessage(socketId, &msg);
        break;
}
*/
