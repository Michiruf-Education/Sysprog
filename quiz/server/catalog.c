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
#include "../common/server_loader_protocol.h"
#include "catalog.h"

int getCatalogCount() {
    // TODO Remove fake of data
    return 2;
}

char *getCatalogNameByIndex(int index) {
    // TODO Remove fake of data
    if (index == 0) {
        return "simple.cat";
    } else {
        return "systemprogrammierung.cat";
    }
}

int loadCatalog(char catalogFile[]) {
    // TODO
}
