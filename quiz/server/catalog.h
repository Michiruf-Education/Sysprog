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

#define SEND_CMD "\n"
#define CATALOG_FILENAME_SIZE 32
#define CATALOG_FILE_EXTENSION ".cat"
#define CATALOGS_MAX_COUNT 16

typedef struct {
    char name[CATALOG_FILENAME_SIZE];
} CATALOG;

int getCatalogCount();

char *getCatalogNameByIndex(int index);

int createCatalogChildProcess(char *catalogPath, char *loaderPath);

int fetchBrowseCatalogs();

int loadCatalog(char catalogFile[]);

// TODO More functions for catalog bla and questions bla

#endif
