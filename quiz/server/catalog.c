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
#include "../common/server_loader_protocol.h"
#include "catalog.h"
#include "../common/util.h"

int getCatalogCount() {
    // TODO Remove fake of data
    return 3;
}

char *getCatalogNameByIndex(int index) {
    // TODO Remove fake of data
    if (index == 0) {
        return "simple.cat";
    } else if (index == 1) {
        return "systemprogrammierung.cat";
    } else {
        return "\0";
    }
}

int loadCatalog(char catalogFile[]) {
    // TODO
    return 0;
}



//CreateCatalogChildProcess
void createCatalogChildProcess(char *catalog_path, char *loader_path) {


    pid_t pid; //Process-ID

    if ((pid = fork()) == (pid_t) -1) {
        errorPrint("Fork-Error: Could not create catalog child-process");

    } else if (pid == 0) { //We are in Child-process
        //excel loader << BROWSE result in catalog-Array
        //excel();
        infoPrint("here we are %s\n",catalog_path);
        infoPrint("here we are %s\n",loader_path);

        int pipe_fds[2];
        int read_fd;
        int write_fd;

        pipe (pipe_fds);
        read_fd = pipe_fds[0];
        write_fd = pipe_fds[1];


    } else { //We are in Parent-process

    }
}
