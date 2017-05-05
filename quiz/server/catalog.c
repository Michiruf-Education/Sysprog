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
#include "../common/server_loader_protocol.h"
#include "catalog.h"

char *selectedCatalogName = NULL;

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

void setSelectedCatalogName(char *name) {
    selectedCatalogName = name;
}

char *getSelectedCatalogName() {
    return selectedCatalogName;
}

int loadCatalog(char catalogFile[]) {
    // TODO
}
