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

#include <sys/types.h>
#include "../common/question.h"

#define RFC_VERSION 9
#define RFC_MAX_MESSAGE_LENGTH 10000
#define RFC_CATALOG_FILE_MAX_LENGTH 32 // TODO FEEDBACK Use limits.h
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

enum {
    ERROR_WARNING_TYPE_WARNING = 0,
    ERROR_WARNING_TYPE_FATAL = 1
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
    char fileName[RFC_CATALOG_FILE_MAX_LENGTH]; // optional
} CATALOG_RESPONSE;

typedef struct {
    char fileName[RFC_CATALOG_FILE_MAX_LENGTH];
} CATALOG_CHANGE;

typedef struct {
    char name[RFC_PLAYER_NAME_LENGTH];
    uint32_t score;
    uint8_t id;
} PLAYER;

typedef struct {
    PLAYER players[RFC_PLAYER_COUNT_MAXIMUM];
} PLAYER_LIST;

typedef struct {
    char catalog[RFC_CATALOG_FILE_MAX_LENGTH]; // optional in server to client responses
} START_GAME;

typedef struct {
    uint8_t selected;
} QUESTION_ANSWERED;

typedef struct {
    uint8_t correct;
} QUESTION_RESULT;

typedef struct {
    uint8_t rank;
    uint32_t score;
} GAME_OVER;

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
    //CATALOG_REQUEST catalogRequest; // Is EMPTY -> useless
    CATALOG_RESPONSE catalogResponse;
    CATALOG_CHANGE catalogChange;
    PLAYER_LIST playerList;
    START_GAME startGame;
    //QUESTION_REQUEST questionRequest; // Is EMPTY -> useless
    QuestionMessage question;
    QUESTION_ANSWERED questionAnswered;
    QUESTION_RESULT questionResult;
    GAME_OVER gameOver;
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

MESSAGE buildQuestion(char question[], char answers[][ANSWER_SIZE], uint8_t timeout);

MESSAGE buildQuestionEmpty(); // For easy access (buildQuestion)

MESSAGE buildQuestionResult(uint8_t correct, int inTime);

MESSAGE buildGameOver(uint8_t rank, uint32_t score);

MESSAGE buildErrorWarning(uint8_t subtype, char message[]);

#endif
