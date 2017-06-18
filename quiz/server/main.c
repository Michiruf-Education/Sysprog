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
#include <fcntl.h>
#include <errno.h>
#include "../common/util.h"
#include "login.h"
#include "score.h"
#include "threadholder.h"
#include "catalog.h"
#include "user.h"

//------------------------------------------------------------------------------
// Types
//------------------------------------------------------------------------------
#define LOCK_FILE "server.lock"

typedef struct {
    char *catalogPath;
    char *loaderPath;
    int port;
} CONFIGURATION;

//------------------------------------------------------------------------------
// Method pre-declaration
//------------------------------------------------------------------------------
static CONFIGURATION initConfiguration();

static bool parseArguments(int argc, char **argv, CONFIGURATION *config);

static void printUsage();

static int createLockFile();

static void closeServerSocket();

static void removeLockFile();

// TODO FEEDBACK Nächste Abgabe: Was passiert wenn ein Thread ein MUTEX hält?
// -> pthread_set_cancel_state Threads nicht abbrechen lassen, wenn MUTEX gehalten wird
// -> pthread cancel, dann join

//------------------------------------------------------------------------------
// Implementations
//------------------------------------------------------------------------------
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

    // Cache the main thread id
    registerMainThread(pthread_self());

    // Change the working directory to the one the executable is located in
    if (chdir(dirname(argv[0])) < 0) {
        errnoPrint("Unable to change the working directory to the executable directory!");
    }

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
    int createLockFileResult = createLockFile();
    if (createLockFileResult < 0) {
        errorPrint("Could not open lock file! Exiting...");
        exit(1);
    } else if (createLockFileResult == 0) {
        errorPrint("Lock file exists (%s)! Cannot start more than one server at once! Exiting...", LOCK_FILE);
        exit(1);
    } else {
        infoPrint("Created lock file: %s", LOCK_FILE);
    }

    // Start the application
    // TODO FEEDBACK Handle errors of functions called below!
    int hasError = 0;
    if (createCatalogChildProcess(config.catalogPath, config.loaderPath) < 0) {
        errorPrint("Cannot create catalog child process!");
        hasError = 1;
    }
    if (!hasError && fetchBrowseCatalogs() < 0) {
        errorPrint("Cannot fetch catalogs!");
        hasError = 1;
    }
    if (!hasError && startLoginThread(&config.port) < 0) {
        errorPrint("Cannot start login thread!");
        hasError = 1;
    }
    if (!hasError && startScoreAgentThread() < 0) {
        errorPrint("Cannot start score agent thread!");
        hasError = 1;
    }


    //Debug-Artur
    //initUserData();
    //addUser("user1", 1);
    //addUser("user2", 2);
    //addUser("user3", 3);

    //calcScoreForUserByID(10, 9, 0);
    //calcScoreForUserByID(10, 3, 1);
    //calcScoreForUserByID(10, 6, 2);

    //printPlayerList();

    //printPlayerListSortedByScore();

    // Shutdown handling:
    // Until a terminating signal the server main-thread shell wait
    // After this we can continue shutting down the server properly
    // But we only need to wait if everything went fine
    if (!hasError) {
        sigset_t signals;
        int signalResult;
        sigemptyset(&signals);
        sigaddset(&signals, SIGINT); // "CTRL-C"
        sigaddset(&signals, SIGTERM); // Termination request
        sigaddset(&signals, SIGQUIT); // Quit from keyboard
        pthread_sigmask(SIG_BLOCK, &signals, NULL);
        while (sigwait(&signals, &signalResult) == -1) {
            errorPrint("Error waiting for signals (or unhandled signal?)!");
        }
        printf("\n"); // Newline after signal (just to have a nicer output)
    } else {
        errorPrint("Shutting down server, because an error occurred.");
    }

    // Shut the server down properly
    cancelAllServerThreads();
    closeServerSocket();
    removeLockFile();
    infoPrint("(Shutdown server) Exiting...");
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
                // NOTE FEEDBACK atoi() kann schief gehen. Benutz strtoul (siehe man, schmeißt Fehler)
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
    errorPrint("        -m        Disable colors in debug output");
}

static int createLockFile() {
    // O_CREAT: Create file, if it does not exist yet
    // O_EXCL:  Fail with EEXIST, if the file already exists
    int fd = open(LOCK_FILE, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        if (errno == EEXIST) {
            return 0; // File already exists, program is probably already running
        } else {
            return -1; // Error opening file
        }
    } else {
        close(fd);
        return 1; // File has been created successfully
    }
}

static void closeServerSocket() {
    infoPrint("Closing server socket");
    close(serverSocketFileDescriptor);
}

static void removeLockFile() {
    int removal = unlink(LOCK_FILE);
    if (removal < 0) {
        errorPrint("Error removing lock file (%s)", LOCK_FILE);
        return;
    }
    infoPrint("Removed lock file: %s", LOCK_FILE);
}
