#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "yel.h"
#include "yem_cli.h"

void yem_not_enough_args() {
    YEL_CRIT("%s%serror:%s the required argument path was not provided.\n\n",
        YEL_BOLD, YEL_UNDERLINE, YEL_RESET_STYLE);
    YEL_ECHO("%s%sUsage:%s yem /path/ (. can be used)\n\n", YEL_BOLD,
        YEL_UNDERLINE, YEL_RESET_STYLE);
    YEL_ECHO("for more information, try `%s--help (-h)%s`\n", YEL_BOLD,
        YEL_RESET_STYLE);
    exit(EXIT_FAILURE);
}

void yem_too_many_args() {
    YEL_CRIT("%s%serror:%s too many arguments provided.\n\n", YEL_BOLD,
        YEL_UNDERLINE, YEL_RESET_STYLE);
    YEL_ECHO("%s%sUsage:%s yem /path/ (. can be used)\n\n", YEL_BOLD,
        YEL_UNDERLINE, YEL_RESET_STYLE);
    YEL_ECHO("for more information, try `%s--help (-h)%s`\n", YEL_BOLD,
        YEL_RESET_STYLE);
    exit(EXIT_FAILURE);
}

void yem_show_help() {
    YEL_ECHO("%s%sUsage:%s yem /path/to/project\n\n", YEL_BOLD, YEL_UNDERLINE,
        YEL_RESET_STYLE);
    YEL_ECHO("%s%sOptions:%s\n", YEL_BOLD, YEL_UNDERLINE, YEL_RESET_STYLE);
    YEL_ECHO("  %s-h, --help\t\t\t%sPrint this help section\n", YEL_BOLD,
        YEL_RESET_STYLE);
    YEL_ECHO("  %s-V, --version\t\t\t%sPrints the version of yem\n", YEL_BOLD,
        YEL_RESET_STYLE);
    exit(EXIT_SUCCESS);
}

void yem_show_version() {
    YEL_ECHO("yem 0.1.0\n");
    exit(EXIT_SUCCESS);
}

void yem_unknown_arg(char* arg) {
    YEL_CRIT("%s%sUnknown option:%s %s\n\n", YEL_BOLD, YEL_UNDERLINE,
        YEL_RESET_STYLE, arg);
    YEL_ECHO(
        "Try `%s--help%s` for more information\n", YEL_BOLD, YEL_RESET_STYLE);
    exit(EXIT_FAILURE);
}

char* yem_parse_args(int argc, char** argv) {
    if (argc < 2) {
        yem_not_enough_args();
    }

    if (argc > 2) {
        yem_too_many_args();
    }

    char* arg = argv[1];

    if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
        yem_show_help();
    }

    if (strcmp(arg, "--version") == 0 || strcmp(arg, "-V") == 0) {
        yem_show_version();
    }

    if (arg[0] == '-') {
        yem_unknown_arg(arg);
    }

    if (strcmp(arg, ".") == 0) {
        char* path = malloc(1024);
        getcwd(path, 1024);
        return path;
    }

    return arg;
}
