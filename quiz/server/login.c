/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * login.c: Implementierung des Logins
 *
 * Dieses Modul beinhaltet die Funktionalität des Login-Threads. Das heißt,
 * hier wird die Schleife zum Entgegennehmen von Verbindungen mittels accept(2)
 * implementiert.
 * Benutzen Sie für die Verwaltung der bereits angemeldeten Clients und zum
 * Eintragen neuer Clients die von Ihnen entwickelten Funktionen aus dem Modul
 * user.
 */

#include "login.h"
