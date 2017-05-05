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

int getCatalogCount();

char *getCatalogNameByIndex(int index);

void setSelectedCatalogName(char *name);

char *getSelectedCatalogName();

int loadCatalog(char catalogFile[]);

void createCatalogChildProcess(char *catalog_path, char *loader_path);

#endif
