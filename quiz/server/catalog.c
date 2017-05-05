/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * catalog.c: Implementierung der Fragekatalog-Behandlung und Loader-Steuerung
 *
 * Implementieren Sie in diesem Modul die Funktionen zum Start des Loaders,
 * zum Auflisten der Fragekataloge und zum Laden des gewählten Fragekataloges.
 */
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <wait.h>
#include "../common/server_loader_protocol.h"
#include "../common/util.h"
#include "catalog.h"

int pipeId[2];

int catalogCount = 0;
CATALOG catalogs[CATALOGS_MAX_COUNT];

static void fetchBrowseCatalogs();

int getCatalogCount() {
    return catalogCount;
}

char *getCatalogNameByIndex(int index) {
    return catalogs[index].name;
}

void createCatalogChildProcess(char *catalogPath, char *loaderPath) {
    pipe(pipeId);

    pid_t pid;
    if ((pid = fork()) == (pid_t) -1) {
        errorPrint("Fork-Error: Could not create catalog child-process");
    } else if (pid == 0) { // Child-process
        if (dup2(pipeId[0], STDIN_FILENO) < 0) {
            errorPrint("Cannot redirect stdin onto pipe!");
        }
        if (dup2(pipeId[1], STDOUT_FILENO) < 0) {
            errorPrint("Cannot redirect stdout onto pipe!");
        }

        int handle = execl(loaderPath, loaderPath, catalogPath, "-d", NULL);
        // We are only getting after execl if there was an error

        if (handle < 0) {
            errorPrint("Error executing loader:");
            errorPrint("\tPath:\t\t%s", loaderPath);
            errorPrint("\tCatalog path:\t%s", catalogPath);
            return;
        }
        exit(1);
    } else { // Parent-process
//        sleep(2);
        int status;
//        waitpid(pid, &status, 0);
        fetchBrowseCatalogs();
    }

    // Safety closing
    close(pipeId[0]);
    close(pipeId[1]);
}

static void fetchBrowseCatalogs() {
    // Send browse command
    if (write(pipeId[1], CMD_BROWSE, sizeof(CMD_BROWSE)) != sizeof(CMD_BROWSE)) {
        errorPrint("Error writing to pipe.");
    };
    if (write(pipeId[1], CMD_SEND, sizeof(CMD_SEND)) != sizeof(CMD_SEND)) {
        errorPrint("Error writing to pipe.");
    };

//    errorPrint("1");
    // Get the result
    char *readBuffer;
    for (int i = 0; (readBuffer = readLine(pipeId[1])) != NULL; i++) {
        if (strstr(readBuffer, CATALOG_FILE_EXTENSION) != NULL) {
            memcpy(catalogs[i].name, readBuffer, strlen(readBuffer));
            catalogCount++;
        }
    }
}

int loadCatalog(char catalogFile[]) {
    // TODO Later
    errorPrint("loadCatalog() not yet implemented.");
    return -1;
}
