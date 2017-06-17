/**
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * threadholder.h: Header zur Verwaltung von Threads
 */
#ifndef RFCHELPER_H
#define RFCHELPER_H

#include "rfc.h"

void broadcastMessage(MESSAGE *message, char *text);

void broadcastMessageWithoutLock(MESSAGE *message, char *text);

void broadcastMessageExcludeOneUser(MESSAGE *message, char *text, int excludedUserId, int lockUserData);

#endif
