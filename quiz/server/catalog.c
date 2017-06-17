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
#include <sys/mman.h>
#include <sys/fcntl.h>
#include "../common/server_loader_protocol.h"
#include "../common/util.h"
#include "catalog.h"

//------------------------------------------------------------------------------
// Fields
//------------------------------------------------------------------------------
static int pipeInFD[2];
static int pipeOutFD[2];

static int catalogCount = 0;
static CATALOG catalogs[CATALOGS_MAX_COUNT];

static int loadedQuestionCount = -1;
static Question *loadedQuestions;

//------------------------------------------------------------------------------
// Implementations
//------------------------------------------------------------------------------
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
    if (write(pipeInFD[1], BROWSE_CMD, sizeof(BROWSE_CMD)) != sizeof(BROWSE_CMD)) {
        errorPrint("Error sending browse command to pipe.");
        return -1;
    }
    if (write(pipeInFD[1], SEND_CMD, sizeof(SEND_CMD)) != sizeof(SEND_CMD)) {
        errorPrint("Error sending send (%s) command to pipe.", SEND_CMD);
        return -2;
    }

    // Get the result
    char *readBuffer;
    for (int i = 0; (readBuffer = readLine(pipeOutFD[0])) != NULL; i++) {
        // Cancel when the end is reached
        if (*readBuffer == '\0') {
            break;
        }

        // Add null termination
        strcat(readBuffer, "\0");

        // Filter files that do not contain catalogs and add them
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
    // NOTE
    // Workaround, because the loaded has a read or write buffer in "queue".
    // Without this we cannot read or write correctly!!!
    // By doing the we get the result the loader does not understand one command.
    char *clear = "\n";
    write(pipeInFD[1], clear, strlen(clear));


    // Send load cmd to load shared memory
    size_t cmdLength = strlen(LOAD_CMD_PREFIX) + strlen(catalogFile) + strlen(SEND_CMD);
    char cmd[cmdLength];
    strcat(cmd, LOAD_CMD_PREFIX);
    strcat(cmd, catalogFile);
    infoPrint("Sending \"%s\" command to loader.", cmd);
    strcat(cmd, SEND_CMD);
    if (write(pipeInFD[1], cmd, strlen(cmd)) != strlen(cmd)) {
        errorPrint("Error sending load command to pipe.");
        return -1;
    }

    // Read response from loader
    char *response = readLine(pipeOutFD[0]);
    infoPrint("Loader response: %s", response);

    char *successPre = "LOADED, SIZE = ";
    if (strncmp(successPre, response, strlen(successPre)) != 0) {
        errorPrint("Loader failure message: %s", response);
        return -2;
    }

    char loadedCategoriesCountString[10];
    memcpy(loadedCategoriesCountString, &response[strlen(successPre)], strlen(response) * sizeof(char));
    loadedQuestionCount = atoi(loadedCategoriesCountString);

    // Open shared memory handle
    int handle = shm_open(SHMEM_NAME, O_RDONLY, 0600);
    if (handle < 0) {
        errorPrint("Could not open shared memory (%s).", SHMEM_NAME);
        return -3;
    }

    // Load questions
    loadedQuestions = mmap(NULL, loadedQuestionCount * sizeof(Question), PROT_READ, MAP_SHARED, handle, 0);

    // Delete the shared memory for future uses
    int deleteShMem = shm_unlink(SHMEM_NAME);
    if (deleteShMem < 0) {
        errorPrint("Could not delete shared memory.");
    }

    return 0;
}

int getLoadedQuestionCount() {
    return loadedQuestionCount;
}

Question *getLoadedQuestions() {
    return loadedQuestions;
}
