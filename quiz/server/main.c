/*
 * Systemprogrammierung
 * Multiplayer-Quiz
 *
 * Server
 *
 * main.c: Hauptprogramm des Servers
 *
 * In diesem Modul werten Sie die Kommandozeile aus und führen Sie
 * (gegebenenfalls auch durch Aufruf von Funktionen aus anderen Modulen)
 * Initialisierungen durch. Außerdem starten Sie hier den Login und den
 * Score-Agent. Auch die Überprüfung (mittels Lock-File), ob bereits eine
 * Instanz des Servers läuft, erfolgt hier.
 */

#include "common/util.h"

int main(int argc, char **argv)
{
	setProgName(argv[0]);
	/* debugEnable() */

	infoPrint("Server Gruppe xy");

	return 0;
}
