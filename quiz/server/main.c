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
#include "../common/util.h"
#include "login.h"

typedef struct configuration {
    char *catalog_path;
    char *loader_path;
    int port;
} CONFIGURATION;

static CONFIGURATION initConfiguration();

static bool parseArguments(int argc, char **argv, CONFIGURATION *config);

static void printUsage();

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


    setProgName(argv[0]);

    // Initialize configuration to work with
    CONFIGURATION config = initConfiguration();

    // Parse arguments to modify configuration
    if (!parseArguments(argc, argv, &config)) {
        printUsage();
        exit(1);
    }

    // Change the working directory to the one the executable is located in
    if (chdir(dirname(argv[0])) < 0) {
        errnoPrint("Unable to change the working directory to the executable directory!");
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

    //Debug-Arthur
    startLoginThread(&config.port);

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
    errorPrint("        -m        Disable colors in debug output");
    exit(1);
}
