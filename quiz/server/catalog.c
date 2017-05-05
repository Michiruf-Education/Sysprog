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

//CreateCatalogChildProcess
void createCatalogChildProcess(char *catalog_path, char *loader_path) {

    pid_t pid; //Process-ID

    if ((pid = fork()) == (pid_t) -1) {
        errorPrint("Fork-Error: Could not create catalog child-process");
    } else if (pid == 0) { // Child-process
        if (dup2(pipeId[0], STDIN_FILENO) == -1) {
            errorPrint("Cannot redirect stdin onto pipe!");
        }
        if (dup2(pipeId[1], STDOUT_FILENO) == -1) {
            errorPrint("Cannot redirect stdout onto pipe!");
        }

        int handle = execl(loaderPath, loaderPath, catalogPath, "-d", NULL);
//        if (handle < 0) {
//            errorPrint("Error executing loader:");
//            errorPrint("\tPath:\t\t%s", loaderPath);
//            errorPrint("\tCatalog path:\t%s", catalogPath);
//            return;
//        }

        fetchBrowseCatalogs();

        close(pipeId[0]);
        close(pipeId[1]);
    } else { // Parent-process
    }
}

void fetchBrowseCatalogs() {
    char *readBuffer;
    int i = 0;
    errorPrint("JAAAAAAAAAAAAAAAAAAAAA");

    // Send browse command
    close(pipeId[0]);
    if (write(pipeId[1], CMD_BROWSE, sizeof(CMD_BROWSE)) < sizeof(CMD_BROWSE)) {
        errorPrint("Error writing to pipe.");
    };
    if (write(pipeId[1], CMD_SEND, sizeof(CMD_SEND)) < sizeof(CMD_SEND)) {
        errorPrint("Error writing to pipe.");
    };

    close(pipeId[1]);
    for (i = 0; (readBuffer = readLine(pipeId[0])) != NULL; i++) {
        debugPrint("%s", readBuffer);
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

