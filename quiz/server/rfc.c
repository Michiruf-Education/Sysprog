/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * rfc.c: Implementierung der Funktionen zum Senden und Empfangen von
 * Datenpaketen gemäß dem RFC
 *
 * Implementieren Sie in diesem Modul Funktionen zum Senden und Empfangen von
 * Netzwerknachrichten gemäß dem RFC. Denken Sie daran, dass Sie beim
 * Empfang nicht von vornehinein wissen, welches Paket ankommt und wie groß
 * dieses ist. Sie müssen also in Ihrem Empfangscode immer zuerst den Header
 * (dessen Größe bekannt ist) empfangen und auswerten.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include "rfc.h"
