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
#include <memory.h>
#include <stdlib.h>
#include "../common/server_loader_protocol.h"
#include "../common/util.h"
#include "catalog.h"

static int pipeInFD[2];
static int pipeOutFD[2];

static int catalogCount = 0;
static CATALOG catalogs[CATALOGS_MAX_COUNT];

int getCatalogCount() {
    return catalogCount;
}

char *getCatalogNameByIndex(int index) {
    return catalogs[index].name;
}

int createCatalogChildProcess(char *catalogPath, char *loaderPath) {
    if (pipe(pipeInFD) == -1 || pipe(pipeOutFD) == -1) {
        errorPrint("Error creating pipes!");
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        errorPrint("Fork-Error: Could not create catalog child-process");
    } else if (pid == 0) { // Child-process
        if (dup2(pipeInFD[0], STDIN_FILENO) < 0) {
            errorPrint("Cannot redirect stdin onto pipe!");
            return -2;
        }
        if (dup2(pipeOutFD[1], STDOUT_FILENO) < 0) {
            errorPrint("Cannot redirect stdout onto pipe!");
            return -3;
        }
        close(pipeInFD[0]);
        close(pipeInFD[1]);
        close(pipeOutFD[0]);
        close(pipeOutFD[1]);

        int handle = execl(loaderPath, loaderPath, catalogPath, "-d", NULL);
        // We are only getting after execl if there was an error

        if (handle < 0) {
            errorPrint("Error executing loader:");
            errorPrint("\tPath:\t\t%s", loaderPath);
            errorPrint("\tCatalog path:\t%s", catalogPath);
            return -4;
        }
        exit(1);
    } else { // Parent-process
        close(pipeInFD[0]);
        close(pipeOutFD[1]);
    }

    return 0;
}

int fetchBrowseCatalogs() {
    // Send browse command
    if (write(pipeInFD[1], CMD_BROWSE, sizeof(CMD_BROWSE)) != sizeof(CMD_BROWSE)) {
        errorPrint("Error writing to pipe.");
        return -1;
    }
    if (write(pipeInFD[1], CMD_SEND, sizeof(CMD_SEND)) != sizeof(CMD_SEND)) {
        errorPrint("Error writing to pipe.");
        return -2;
    }

    // Get the result
    char *readBuffer;
    for (int i = 0; (readBuffer = readLine(pipeOutFD[0])) != NULL; i++) {
        if (*readBuffer == '\0') {
            break;
        }
        if (strstr(readBuffer, CATALOG_FILE_EXTENSION) != NULL) {
            memcpy(catalogs[i].name, readBuffer, strlen(readBuffer));
            catalogCount++;
        }
    }

    // Fake the empty entry
    CATALOG emptyCatalog;
    emptyCatalog.name[0] = '\0';
    catalogs[catalogCount++] = emptyCatalog;

    return 0;
}

int loadCatalog(char catalogFile[]) {
    // TODO Later
    errorPrint("loadCatalog() not yet implemented.");
    return -1;
}
