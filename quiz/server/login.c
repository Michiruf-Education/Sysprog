/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * login.c: Implementierung des Logins
 *
 * Dieses Modul beinhaltet die Funktionalität des Login-Threads. Das heißt,
 * hier wird die Schleife zum Entgegennehmen von Verbindungen mittels accept(2)
 * implementiert.
 * Benutzen Sie für die Verwaltung der bereits angemeldeten Clients und zum
 * Eintragen neuer Clients die von Ihnen entwickelten Funktionen aus dem Modul
 * user.
 */
#include "login.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "vardefine.h"
#include "user.h"
#include "../common/util.h"
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include "clientthread.h"
#include "threadholder.h"

//------------------------------------------------------------------------------
// Method pre-declaration
//------------------------------------------------------------------------------
static int startLoginListener(int *port);

//------------------------------------------------------------------------------
// Fields
//------------------------------------------------------------------------------
static pthread_t loginThreadId = 0;

static int loginIsEnable = -1;

//------------------------------------------------------------------------------
// Implementations
//------------------------------------------------------------------------------
//Main - start function for login thread
int startLoginThread(int *port) {
    int result = pthread_create(&loginThreadId, NULL, (void *) &startLoginListener, (void *) port);
    if (result != 0) {
        errorPrint("Can't create Login thread");
        return -1;
    }

    infoPrint("Login thread created successfully");
    registerThread(loginThreadId);
    return 0;
}

void enableLogin() {
    loginIsEnable = -1;
}

void disableLogin() {
    loginIsEnable = 1;
}

//return -1 on error
static int startLoginListener(int *port) { // TODO FEEDBACK: Should return void* and get void* pointer for port
    // Initialise UserData
    initUserData();

    infoPrint("Starting login listener...");

    // Client descriptor
    int client_sock;

    // Socket create type AF_INET IPv4, TCP
    serverSocketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    // Allow the socket to be reused immediately
    int sockOptOn = 1;
    setsockopt(serverSocketFileDescriptor, SOL_SOCKET, SO_REUSEADDR, &sockOptOn, sizeof(sockOptOn));
    // Print error
    if (serverSocketFileDescriptor < 0) {
        errorPrint("Could not create listen socket for server");
        exit(1);
    }

    //specify IP-address and listening port
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(*port);

    //Socket bind to local IP and port
    if (bind(serverSocketFileDescriptor, (const struct sockaddr *) &addr, sizeof(addr)) < 0) {
        errorPrint("Could not bind socket to address");
        exit(1);
    }

    // Listen to connections
    if (listen(serverSocketFileDescriptor, MAXCONNECTIONS) < 0) {
        errorPrint("Could not listen for client connections");
        exit(1);
    }
    infoPrint("Bind socket to local IP on Port: %d, and listening...", *port);

    while (1) {
        //waits for client connection
        //accept() - blockiert und wartet bis eine Verbindung vom Client aufgebaut wird
        //client_sock beinhaltet den Socket-Deskriptor des Clients
        client_sock = accept(serverSocketFileDescriptor, NULL, NULL);
        // In case the socket was closed, do not print an error
        if (errno == EBADF) {
            return -1;
        }
        // Print error
        if (client_sock < 0) {
            errorPrint("Could not accept client connection");
            return -1;
        }

        if (getUserAmount() >= MAXUSERS || loginIsEnable >= 0) {
            MESSAGE errorWarning = buildErrorWarning(
                    ERROR_WARNING_TYPE_FATAL,
                    "Game running or maximum user amount reached, please try again later...");
            if (sendMessage(client_sock, &errorWarning) < 0) {
                errorPrint("Unable to send game running or maximum users reached error warning!");
            }
            errorPrint("Game running or maximum user amount reached, please try again later...");
            continue;
        }

        MESSAGE message;
        char username[USERNAMELENGTH];

        if (receiveMessage(client_sock, &message) < 0 && validateMessage(&message) < 0) {
            errorPrint("Error: Message not received or malformed");
            continue;

        }

        if (message.header.type != TYPE_LOGIN_REQUEST) {
            errorPrint("Error: Message received but type not login request");
            continue;
        }

        memcpy(username, message.body.loginRequest.name, USERNAMELENGTH);

        if (addUser(username, client_sock) < 0) {
            errorPrint("Error: User could not be added to user data");
            continue;
        }

        //Message send
        int clientID = getUserIDbyClientSocket(client_sock);
        //infoPrint("Client-ID: %d",clientID);
        MESSAGE sendmessage = buildLoginResponseOk(message.body.loginRequest.rfcVersion, MAXUSERS,
                                                   (__uint8_t) clientID);

        if (sendMessage(client_sock, &sendmessage) < 0) {
            errorPrint("Error: Message send failure");
            continue;
        }

        printUSERDATA();
        startClientThread(clientID);
    }
}
