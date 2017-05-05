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

#define LOCK_FILE "server.lock"

typedef struct configuration {
    char *catalog_path;
    char *loader_path;
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
    // TODO Remove program args fake
    char *args[8];
    args[0] = "server";
    args[1] = "-c";
    args[2] = "/media/sf_quiz/";
    args[3] = "-l";
    args[4] = "/media/sf_quiz/bin/loader";
    args[5] = "-p";
    args[6] = argv[2];
    args[7] = "-d";
    argv = args;
    argc = 8;
    // TODO End remove fake args


    setProgName(basename(argv[0]));

    // Show console logging immediately
    setvbuf(stdout, NULL, _IOLBF, 0);

    // Change the working directory to the one the executable is located in
    if (chdir(dirname(argv[0])) < 0) {
        errnoPrint("Unable to change the working directory to the executable directory!");
    }

    // Lock file handling
    if (checkLockFileExists() >= 0) {
        errorPrint("Lock file exists (%s)! Cannot start more than one server at once! Exiting...", LOCK_FILE);
        exit(1);
    }
    createLockFile();

    // Set shutdown hooks
    signal(SIGABRT, shutdownServer);
    signal(SIGINT, shutdownServer);
    signal(SIGTERM, shutdownServer);

    // Initialize and parse arguments for configuration
    CONFIGURATION config = initConfiguration();
    if (!parseArguments(argc, argv, &config)) {
        printUsage();
        exit(1);
    }
    debugPrint("Configuration:");
    debugPrint("    Catalog-path:\t%s", config.catalog_path);
    debugPrint("    Loader-path:\t%s", config.loader_path);
    debugPrint("    Port:\t\t%d", config.port);

    // TODO
    //start_loader();
    //browserCatalogs();
    //signal(SIGINT, ShutDownServer);
    //createIPCs();

    // TODO LOCK FILE!!!

    startLoginThread(&config.port);
    startScoreAgentThread();

    while (1) {

    }

    return 0;
}

static CONFIGURATION initConfiguration() {
    CONFIGURATION config;
    config.catalog_path = "./";
    config.loader_path = "./loader";
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
                config->catalog_path = optarg;
                categorySet = true;
                break;
            case 'l':
                config->loader_path = optarg;
                loaderSet = true;
                break;
            case 'p':
                config->port = atoi(optarg);
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
    exit(1);
}

static void shutdownServer() {
    // TODO cancelAllServerThreads();
    removeLockFile();
    exit(0);
}

static int checkLockFileExists() {
    return access(LOCK_FILE, F_OK);
}

static void createLockFile() {
    FILE *lockFile = fopen(LOCK_FILE, "w+");
    infoPrint("Creating lock file: %s", LOCK_FILE);
    if (lockFile == NULL) {
        errorPrint("Could not create log file! Exiting...");
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
