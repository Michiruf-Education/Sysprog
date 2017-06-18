/**
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * rfchelper.c: Implementierung für Broadcast-Messages.
 */
#include "rfchelper.h"
#include "user.h"
#include "../common/util.h"

//------------------------------------------------------------------------------
// Implementations
//------------------------------------------------------------------------------
void broadcastMessage(MESSAGE *message, char *text) {
    broadcastMessageExcludeOneUser(message, text, -1, 1);
}

void broadcastMessageWithoutLock(MESSAGE *message, char *text) {
    broadcastMessageExcludeOneUser(message, text, -1, 0);
}

void broadcastMessageExcludeOneUser(MESSAGE *message, char *text, int excludedUserId, int doLockUserData) {
    // We may need to lock user data because it may change during iteration
    if (doLockUserData) {
        lockUserData();
    }

    // Send broadcast
    for (int i = 0; i < getUserAmount(); i++) {
        USER user = getUserByIndex(i);
        if (user.id == excludedUserId) {
            continue;
        }

        if (sendMessage(user.clientSocket, message) < 0) {
            errorPrint(text, user.username, user.id);
        }
    }

    // Unlock after locking
    if (doLockUserData) {
        unlockUserData();
    }
}
