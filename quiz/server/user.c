/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * user.c: Implementierung der User-Verwaltung
 *
 * Die angemeldeten Clients werden in einem Array oder einer verketteten
 * Liste aus C-Strukturen verwaltet. Dieses Modul enthält die Funktionen
 * zum Verwalten dieser Daten. Dies beinhaltet das Eintragen und Löschen
 * von Clients und das Iterieren über die Einträge.
 * Da diese Datenstruktur von mehreren Threads gleichzeitig verwendet wird,
 * ist auf die korrekte Synchronisierung zu achten!
 */

#include "user.h"
