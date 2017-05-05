/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * catalog.h: Header für die Katalogbehandlung und Loader-Steuerung
 */
#ifndef CATALOG_H
#define CATALOG_H

#define CMD_SEND "\n"
#define CMD_BROWSE "BROWSE"
#define CATALOG_FILENAME_SIZE 32
#define CATALOG_FILE_EXTENSION ".cat"
#define CATALOGS_MAX_COUNT 16

typedef struct {
    char name[CATALOG_FILENAME_SIZE];
} CATALOG;

int getCatalogCount();

char *getCatalogNameByIndex(int index);

void createCatalogChildProcess(char *catalogPath, char *loaderPath);

void fetchBrowseCatalogs();

int loadCatalog(char catalogFile[]);

#endif
