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



/**
 * TODO comment
 * Wenn man ein Feld vom typ uint_16 benutzt, einmal die byte order per ntohs() umdrehen
 * Wenn man ein Feld vom typ uint_32+ benutzt, einmal die byte order per ntohl() umdrehen
 *
 *
 * @param socketId
 * @param message
 * @return
 */
ssize_t receiveMessage(int socketId, MESSAGE *message) {
    ssize_t headerSize = recv(socketId, &message->header, sizeof(message->header), MSG_WAITALL);
    if(headerSize == sizeof(message->header)) {
        uint16_t bodyLength = ntohs(message->header.length);
        ssize_t bodySize = recv(socketId, &message->body, bodyLength,  MSG_WAITALL);
        if(bodySize == bodyLength) {
            return headerSize + bodySize;
        }
    }
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
        case TYPE_LOGIN_RESPONSE_SUCCESSFUL: // TODO validate from here on
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
        case TYPE_QUESTION_ANSWERED :
            break;
        case TYPE_QUESTION_RESULT :
            break;
        case TYPE_GAME_OVER :
            break;
        case TYPE_ERROR_WARNING :
            break;
        default:
            errorPrint("RFC type is unknown");
            return -1;
    }

    return 1;
}

void sendLoginResponseSuccessful(int clientSocket, LOGIN_REQUEST loginRequest, int maxPlayerCount, int clientId) {
#pragma pack(1)
    typedef struct send_data {
        HEADER header;
        uint8_t rfcVersion;
        uint8_t maxPlayers;
        uint8_t clientId;
    };
#pragma pack(0)
    struct send_data sendData;
    sendData.header.type = TYPE_LOGIN_RESPONSE_SUCCESSFUL;
    sendData.header.length = sizeof(struct send_data) - sizeof(HEADER);
    sendData.rfcVersion = (uint8_t) loginRequest.rfcVersion;
    sendData.maxPlayers = (uint8_t) maxPlayerCount;
    sendData.clientId = (uint8_t) clientId;

    send(clientSocket, &sendData, sizeof(struct send_data), 0);
}

CATALOG_REQUEST parseCatalogRequest(uint8_t message[]);

void sendCatalogResponse(int clientSocket, /* nullable */ char catalogFileName[]);

CATALOG_CHANGE parseCatalogChange(uint8_t message[]);

void sendCatalogChange(int clientSocket, CATALOG_CHANGE catalogChange);

void sendPlayerList(int clientSocket, PLAYER players[]);

START_GAME parseStartGame(uint8_t message[]);

void sendStartGame(int clientSocket, START_GAME startGame);
