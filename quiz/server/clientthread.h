/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * clientthread.h: Header für den Client-Thread
 */
#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

enum {
    GAME_STATE_PREPARATION = 1,
    GAME_STATE_GAME_RUNNING = 2,
    GAME_STATE_FINISHED = 3,
    GAME_STATE_ABORTED = 4
};

int startClientThread(int userId);

#endif
