/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * rfc.c: Implementierung der Funktionen zum Senden und Empfangen von
 * Datenpaketen gemäß dem RFC
 *
 * Implementieren Sie in diesem Modul Funktionen zum Senden und Empfangen von
 * Netzwerknachrichten gemäß dem RFC. Denken Sie daran, dass Sie beim
 * Empfang nicht von vornehinein wissen, welches Paket ankommt und wie groß
 * dieses ist. Sie müssen also in Ihrem Empfangscode immer zuerst den Header
 * (dessen Größe bekannt ist) empfangen und auswerten.
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include "rfc.h"
#include "../common/util.h"

#ifndef DIRECTION_RECEIVE
#define DIRECTION_RECEIVE 1
#endif
#ifndef DIRECTION_SEND
#define DIRECTION_SEND 2
#endif

static void fixRFCHeader(MESSAGE *message, int direction) {
    message->header.length = direction == DIRECTION_RECEIVE ?
                             ntohs(message->header.length) :
                             htons(message->header.length);
}

static void fixRFCBody(MESSAGE *message, int direction) {
    /**
     * TODO comment
     * Wenn man ein Feld vom typ uint_16 benutzt, einmal die byte order per ntohs() umdrehen
     * Wenn man ein Feld vom typ uint_32+ benutzt, einmal die byte order per ntohl() umdrehen
     */
    switch (message->header.type) {
        case TYPE_LOGIN_REQUEST:
            if (direction == DIRECTION_RECEIVE) {
                message->body.loginRequest.name[message->header.length - 1] = '\0';
            };
            break;
        case TYPE_LOGIN_RESPONSE_OK:
            break;
        case TYPE_CATALOG_REQUEST:
            break;
        case TYPE_CATALOG_RESPONSE:
            break;
        case TYPE_CATALOG_CHANGE:
            if (direction == DIRECTION_RECEIVE) {
                message->body.catalogChange.fileName[message->header.length] = '\0';
            };
            break;
        case TYPE_PLAYER_LIST:
            if (direction == DIRECTION_RECEIVE) {
                int playerCount = message->header.length / sizeof(PLAYER);
                for (int i = 0; i < playerCount; i++) {
                    message->body.playerList.players[i].score = ntohl(message->body.playerList.players[i].score);
                }
            } else {
                int playerCount = message->header.length / sizeof(PLAYER);
                for (int i = 0; i < playerCount; i++) {
                    message->body.playerList.players[i].score = htonl(message->body.playerList.players[i].score);
                }
            }
            break;
        case TYPE_START_GAME:
            if (direction == DIRECTION_RECEIVE) {
                message->body.startGame.catalog[message->header.length] = '\0';
            };
            break;
        case TYPE_QUESTION_REQUEST:
            break;
        case TYPE_QUESTION:
            break;
        case TYPE_QUESTION_ANSWERED:
            break;
        case TYPE_QUESTION_RESULT:
            break;
        case TYPE_GAME_OVER:
            break;
        case TYPE_ERROR_WARNING:
            if (direction == DIRECTION_RECEIVE) {
                message->body.errorWarning.message[message->header.length] = '\0';
            };
            break;
        default:
            break;
    }
}

ssize_t receiveMessage(int socketId, MESSAGE *message) {
    ssize_t headerSize = recv(socketId, &message->header, sizeof(message->header), MSG_WAITALL);
    if (headerSize == sizeof(message->header)) {
        fixRFCHeader(message, DIRECTION_RECEIVE);
        uint16_t bodyLength = message->header.length;
        debugPrint("====== GOT MESSAGE ======");
        debugPrint("Type:\t\t%d", message->header.type);
        debugPrint("Header size: \t\t%zu", headerSize);
        debugPrint("Header's body length: \t%lu", (unsigned long) bodyLength);
        ssize_t bodySize = recv(socketId, &message->body, bodyLength, MSG_WAITALL);
        debugPrint("Read body length: \t%zu", bodySize);
        if (bodySize == bodyLength) {
            fixRFCBody(message, DIRECTION_RECEIVE);
            debugPrint("//////// SUCCESS ////////");
            return headerSize + bodySize;
        }
    }

    debugPrint("\\\\\\\\\\\\\\\\ FAILURE \\\\\\\\\\\\\\\\");
    return -1;
}

int validateMessage(MESSAGE *message) {
    switch (message->header.type) {
        case TYPE_LOGIN_REQUEST:
            if (message->body.loginRequest.rfcVersion != RFC_VERSION) {
                errorPrint("RFC version of login request is wrong. Expected %d, got %d.", RFC_VERSION,
                           message->body.loginRequest.rfcVersion);
                return -1;
            }
            if (strlen(message->body.loginRequest.name) > RFC_PLAYER_NAME_LENGTH) {
                errorPrint("Player name exceeded %d allowed characters.", RFC_PLAYER_NAME_LENGTH);
                return -2;
            }
            break;
        case TYPE_LOGIN_RESPONSE_OK: // TODO validate from here on
            break;
        case TYPE_CATALOG_REQUEST:
            break;
        case TYPE_CATALOG_RESPONSE:
            break;
        case TYPE_CATALOG_CHANGE:
            break;
        case TYPE_PLAYER_LIST:
            break;
        case TYPE_START_GAME:
            break;
        case TYPE_QUESTION_REQUEST:
            break;
        case TYPE_QUESTION:
            break;
        case TYPE_QUESTION_ANSWERED:
            break;
        case TYPE_QUESTION_RESULT:
            break;
        case TYPE_GAME_OVER:
            break;
        case TYPE_ERROR_WARNING:
            break;
        default:
            errorPrint("RFC type is unknown");
            return -1;
    }

    return 1;
}

ssize_t sendMessage(int socketId, MESSAGE *message) {
    // Get that header length before reversing byte orders
    uint16_t headerLength = message->header.length;
    size_t completeLength = sizeof(HEADER) + headerLength;

    debugPrint("==== SENDING MESSAGE ====");
    debugPrint("Type:\t\t\t%d", message->header.type);
    debugPrint("Header's body length: \t%lu", (unsigned long) headerLength);
    debugPrint("Complete length: \t%zu", completeLength);

    // Reverse byte order (do body first because we can then access the header)
    fixRFCBody(message, DIRECTION_SEND);
    fixRFCHeader(message, DIRECTION_SEND);

    ssize_t sendSize = send(socketId, message, completeLength, 0);
    debugPrint("Sent length: \t\t%zu", sendSize);

    // Undo reverse of byte order (important for sending message more than once!)
    fixRFCHeader(message, DIRECTION_RECEIVE);
    fixRFCBody(message, DIRECTION_RECEIVE);

    if (sendSize == completeLength) {
        debugPrint("//////// SUCCESS ////////");
        return sendSize;
    }

    debugPrint("\\\\\\\\\\\\\\\\ FAILURE \\\\\\\\\\\\\\\\");
    return -1;
}

MESSAGE buildLoginResponseOk(uint8_t rfcVersion, uint8_t maxPlayerCount, uint8_t clientId) {
    MESSAGE msg;
    msg.header.type = TYPE_LOGIN_RESPONSE_OK;
    msg.header.length = 3;
    msg.body.loginResponseOk.rfcVersion = rfcVersion;
    msg.body.loginResponseOk.maxPlayers = maxPlayerCount;
    msg.body.loginResponseOk.clientId = clientId;
    return msg;
}

MESSAGE buildCatalogResponse(/* nullable */ char catalogFileName[]) {
    MESSAGE msg;
    msg.header.type = TYPE_CATALOG_RESPONSE;
    msg.header.length = (uint16_t) strlen(catalogFileName);
    memcpy(msg.body.catalogResponse.fileName, catalogFileName, strlen(catalogFileName));
    return msg;
}

MESSAGE buildCatalogChange(char catalogFileName[]) {
    MESSAGE msg;
    msg.header.type = TYPE_CATALOG_CHANGE;
    msg.header.length = (uint16_t) strlen(catalogFileName);
    memcpy(msg.body.catalogChange.fileName, catalogFileName, strlen(catalogFileName));
    return msg;
}

MESSAGE buildPlayerList(PLAYER players[], int playerCount) {
    if (sizeof(PLAYER) != 37) {
        errorPrint("Size of PLAYER struct is not 37 anymore!");
    }

    MESSAGE msg;
    msg.header.type = TYPE_PLAYER_LIST;
    msg.header.length = (uint16_t) (playerCount * sizeof(PLAYER));
    memcpy(msg.body.playerList.players, players, sizeof(PLAYER) * playerCount);
    return msg;
}

MESSAGE buildStartGame(/* nullable */ char catalogFileName[]) {
    MESSAGE msg;
    msg.header.type = TYPE_START_GAME;
    msg.header.length = (uint16_t) strlen(catalogFileName);
    memcpy(msg.body.startGame.catalog, catalogFileName, strlen(catalogFileName));
    return msg;
}

MESSAGE buildErrorWarning(uint8_t subtype, char message[]) {
    MESSAGE msg;
    msg.header.type = TYPE_ERROR_WARNING;
    msg.header.length = (uint16_t) (strlen(message) + 1);
    msg.body.errorWarning.subtype = subtype;
    memcpy(msg.body.errorWarning.message, message, strlen(message));
    return msg;
}
