/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * clientthread.c: Implementierung des Client-Threads
 *
 * In diesem Modul wird die an phread_create(3) übergebene Threadfunktion für
 * den Client-Thread implementiert. Bitte nutzen Sie modulgebundene (static)
 * Hilfsfunktionen, um die Implementierung übersichtlich zu halten und
 * schreiben Sie nicht alles in eine einzige große Funktion.
 * Verwenden Sie zum Senden und Empfangen von Nachrichten die von Ihnen
 * definierten Funktionen und Strukturen aus dem RFC-Modul.
 * Benutzen Sie für den Zugriff auf die User-Liste das Modul user.
 * Zum Ansteuern des Loaders und zur Verwaltung der Fragekataloge sollen
 * die Funktionen aus dem Modul catalog verwendet werden.
 */

#include "clientthread.h"
