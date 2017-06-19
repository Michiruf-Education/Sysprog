/**
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * rfchelper.h: Header für broadcast-messages.
 */
#ifndef RFCHELPER_H
#define RFCHELPER_H

#include "rfc.h"

void broadcastMessage(MESSAGE *message, char *text);

void broadcastMessageWithoutLock(MESSAGE *message, char *text);

void broadcastMessageExcludeOneUser(MESSAGE *message, char *text, int excludedUserId, int lockUserData);

#endif
