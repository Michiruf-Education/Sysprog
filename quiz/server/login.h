/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * login.h: Header für das Login
 */

#ifndef LOGIN_H
#define LOGIN_H

int startLoginThread(int *port);

int startLoginListener(int *port);

void enableLogin();

void disableLogin();

#endif