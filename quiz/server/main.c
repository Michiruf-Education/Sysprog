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
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <unistd.h>
#include <libgen.h>
#include <stdio.h>
#include <sys/signal.h>
#include "../common/util.h"
#include "login.h"
#include "score.h"
#include "threadholder.h"
#include "catalog.h"

#define LOCK_FILE "server.lock"

typedef struct {
    char *catalogPath;
    char *loaderPath;
    int port;
} CONFIGURATION;

static CONFIGURATION initConfiguration();

static bool parseArguments(int argc, char **argv, CONFIGURATION *config);

static void printUsage();

static void shutdownServer();

static int checkLockFileExists();

static void createLockFile();

static void removeLockFile();

int main(int argc, char **argv) {
//    // TODO Remove program args fake
//    char *args[8];
//    args[0] = "server";
//    args[1] = "-c";
//    args[2] = "/media/sf_quiz/catalogs/";
//    args[3] = "-l";
//    args[4] = "/media/sf_quiz/bin/loader";
//    args[5] = "-p";
//    args[6] = argv[2];
//    args[7] = "-d";
//    argv = args;
//    argc = 8;
//    // TODO End remove fake args


    setProgName(basename(argv[0]));

    // Show console logging immediately
    setvbuf(stdout, NULL, _IOLBF, 0);

    // Change the working directory to the one the executable is located in
    if (chdir(dirname(argv[0])) < 0) {
        errnoPrint("Unable to change the working directory to the executable directory!");
    }

    // Set shutdown hooks
    signal(SIGABRT, shutdownServer);
    signal(SIGINT, shutdownServer);
    signal(SIGTERM, shutdownServer);
    atexit(shutdownServer);

    // Initialize and parse arguments for configuration
    CONFIGURATION config = initConfiguration();
    if (!parseArguments(argc, argv, &config)) {
        printUsage();
        infoPrint("Exiting...");
        exit(1);
    }
    debugPrint("Configuration:");
    debugPrint("    Catalog-path:\t%s", config.catalogPath);
    debugPrint("    Loader-path:\t%s", config.loaderPath);
    debugPrint("    Port:\t\t%d", config.port);

    // Lock file handling
    // TODO Wenn 2 Server gleichzeitig gestartet werden, läufts dennoch
    // -> open() mit Parameter von moodle (O_EXCL ist wichtig)
    if (checkLockFileExists() >= 0) {
        errorPrint("Lock file exists (%s)! Cannot start more than one server at once! Exiting...", LOCK_FILE);
        exit(1);
    }
    createLockFile();

    // Start the application
    // TODO FEEDBACK Handle errors!
    createCatalogChildProcess(config.catalogPath, config.loaderPath);
    fetchBrowseCatalogs();
    startLoginThread(&config.port);
    startAwaitScoreAgentThread();

    // TODO FEEDBACK We could use sigwait() to wait for any signal instead of the cleanup signal handler above
    // Because there are functions that may not be used in signal handler
    // If we wait for a signal here, we can just continue with cleanup stuff like normal

    // TODO FEEDBACK Nächste Abgabe: Was passiert wenn ein Thread ein MUTEX hält?
    // -> pthread_set_cancel_state Threads nicht abbrechen lassen, wenn MUTEX gehalten wird
    // -> pthread cancel, dann join

    infoPrint("Exiting regular (main done)...");
    return 0;
}

static CONFIGURATION initConfiguration() {
    CONFIGURATION config;
    config.catalogPath = "./";
    config.loaderPath = "./loader";
    config.port = 8000;
    return config;
}

static bool parseArguments(int argc, char **argv, CONFIGURATION *config) {
    int categorySet = false;
    int loaderSet = false;
    int portSet = false;

    int param;
    while ((param = getopt(argc, argv, "c:l:p:dm")) != -1) {
        switch (param) {
            case 'c':
                config->catalogPath = optarg;
                categorySet = true;
                break;
            case 'l':
                config->loaderPath = optarg;
                loaderSet = true;
                break;
            case 'p':
                config->port = atoi(optarg); // TODO FEEDBACK atoi kann schiefgehen. Benutz strtoul (siehe man, schmeißt Fehler)
                portSet = true;
                break;
            case 'd':
                debugEnable();
                break;
            case 'm':
                styleDisable();
                break;
            default:
                // Fail safe check (because only allowed arguments shell get checked)
                return false;
        }
    }

    return categorySet && loaderSet && portSet;
}

static void printUsage() {
    errorPrint("Usage:  %s -c CATALOG_PATH -l LOADER_PATH -p PORT [-d] [-m]", getProgName());
    errorPrint("        -c        Specify catalog location. Required.");
    errorPrint("        -l        Specify loader location. Required.");
    errorPrint("        -p        Specify port. Required");
    errorPrint("        -d        Enable debug output");
    errorPrint("CURRENTLY ONLY WORKS WITH DEBUG ENABLED! SEE README.txt!"); // TODO Remove later
    errorPrint("        -m        Disable colors in debug output");
}

static void shutdownServer() {
    infoPrint(" "); // Newline
    cancelAllServerThreads();
    removeLockFile();
    infoPrint("(Shutdown server) Exiting...");
    exit(0);
}

static int checkLockFileExists() {
    return access(LOCK_FILE, F_OK);
}

static void createLockFile() {
    FILE *lockFile = fopen(LOCK_FILE, "w+");
    infoPrint("Creating lock file: %s", LOCK_FILE);
    if (lockFile == NULL) {
        errorPrint("Could not create lock file! Exiting...");
        exit(1);
    }
    fclose(lockFile);
}

static void removeLockFile() {
    int removal = unlink(LOCK_FILE);
    infoPrint("Removing lock file: %s", LOCK_FILE);
    if (removal < 0) {
        errorPrint("ERROR removing lock file (%s)", LOCK_FILE);
    }
}
