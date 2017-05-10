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
#include "rfc.h"
#include "clientthread.h"
#include "threadholder.h"

pthread_t loginThreadID = 0;

//Main - start function for login thread
int startLoginThread(int *port) {

    int err;
    err = pthread_create(&loginThreadID, NULL, (void *) &startLoginListener, (void *) port);
    if (err == 0) {
        infoPrint("Login thread created successfully");
        registerThread(loginThreadID);
    } else {
        errorPrint("Can't create Login thread");
    }
    return err;
}

//TODO gameMode implementieren
//return -1 => VorbereitungsPhase
//return 1 => Spiel läuft
//wenn StartGame (STG) c=>s dann auf Spiel läuft wechseln
int getGameMode() {
    return -1;
}

//return -1 on error
int startLoginListener(int *port) { // TODO FEEDBACK: Should return void* and get void* pointer for port

    //Initialise UserData
    initUserData();

    infoPrint("Starting login listener...");

    //Client Deskriptor
    int client_sock;

    //Socket create type AF_INET IPv4, TCP
    const int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0) {
        errorPrint("Could not create listen socket for server");
        return -1;
    }

    //specify IP-address and listening port
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(*port);

    //Socket bind to local IP and port
    if (bind(listen_sock, (const struct sockaddr *) &addr, sizeof(addr)) < 0) {
        errorPrint("Could not bind socket to address");
        exit(1);
        //return -1;
    }

    //Listen to Connections, MAXCONNECTIONS 4
    if (listen(listen_sock, MAXCONNECTIONS) < 0) {
        errorPrint("Could not accept client connection");
        return -1;
    } else {
        infoPrint("Bind socket to local IP on Port: %d, and listening...", *port);
    }

    while (1) {
        //waits for client connection
        //accept() - blockiert und wartet bis eine Verbindung vom Client aufgebaut wird
        //client_sock beinhaltet den Socket-Deskriptor des Clients
        client_sock = accept(listen_sock, NULL, NULL);

        if (client_sock < 0) {
            errorPrint("Could not accept client connection");
            return -1;
        }

        if (getUserAmount() < MAXUSERS && getGameMode() < 0) {

            MESSAGE message;
            char username[USERNAMELENGTH];

            if (receiveMessage(client_sock, &message) >= 0 && validateMessage(&message) >= 0) {

                if (message.header.type == TYPE_LOGIN_REQUEST) {

                    memcpy(username, message.body.loginRequest.name, USERNAMELENGTH);

                    if (addUser(username, client_sock) >= 0) {
                        //Message send
                        int clientID = getClientIDforUser(client_sock);
                        //infoPrint("Client-ID: %d",clientID);
                        MESSAGE sendmessage = buildLoginResponseOk(message.body.loginRequest.rfcVersion, MAXUSERS,
                                                                   (__uint8_t) clientID);

                        if (sendMessage(client_sock, &sendmessage) >= 0) {
                            printUSERDATA();
                            startClientThread(clientID);
                        } else {
                            errorPrint("Error: Message send failure");
                            //TODO sendFatalErrorMessage();
                        }

                    } else {
                        errorPrint("Error: User could not be added to Userdata");
                    }

                }
            } else {
                errorPrint("Error: Message not received or malformed");
            }


        } else {
            MESSAGE errorWarning = buildErrorWarning(
                    ERROR_WARNING_TYPE_FATAL,
                    "Game running or maximum user amount reached, please try again later...");
            if (sendMessage(client_sock, &errorWarning) < 0) {
                errorPrint("Unable to send error warning to!");
            }
            errorPrint("Error: Game running or maximum user amount reached, please try again later...");
        }
    }
}
