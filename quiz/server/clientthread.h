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
    GAME_STATE_FINISHED = 3
};

int initializeClientThread(int userId);

#endif
