#include "auxiliary.h"

void printUsage(char *argv[]) {
  fprintf(stderr, "Usage: %s -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n", argv[0]);
}

void invalidArgs(char *argv[]) {
  fprintf(stderr, "Invalid Arguments!\n");
  printUsage(argv);
  exit(1);
}

void invalidBArg(char *argv[], char *val) {
  fprintf(stderr, "Invalid -B Argument! %s is not a number\n", val);
  printUsage(argv);
  exit(1);
}

void invalidOption(char *argv[], char *option, int r) {
  fprintf(stderr, "%s is not a valid option!\n", option);
  printUsage(argv);
  exit(r);
}

int validOption(char *option) {

    if (!strcmp(option, "a") || !strcmp(option, "b") || !strcmp(option, "B")
        || !strcmp(option, "L") || !strcmp(option, "S") || !strcmp(option, "--max-depth")
        || !strcmp(option, "--block-size=") || !strcmp(option, "--all") || !strcmp(option, "--bytes")
        || !strcmp(option, "--dereference") || !strcmp(option, "--separate-dirs")) {
        return true;
    }
    return false;
}

void  makeOptinsDiff(char *option, char *specialOption, char *check) {

    int l = 0;

    while (option[l+2] != '\0'){
        specialOption[l] = option[l+2];
        l++;
    }
    specialOption[l] = '\0';
    l = 0;
    while(option[l] != '\0' && option[l+1] != '=') {
        check[l] = option[l];
        l++;
    }
    check[l] = '\0';
}

void buildOption(char *argv, char *option, char *aux) {

    int l=0;

    while(argv[l+1] != '\0') {
        option[l] = argv[l+1];
        l++;
    }
    option[l] = '\0';

    aux[0] = option[0];
    aux[1] = option[1];
    aux[2] = '\0';
}

void sort(char *options[], int size) {

    char aux[15];

    for(int i=0; i<size; i++) {
        for(int j=i+1; j<size; j++) {
            if(strcmp(options[i],options[j]) > 0) {
                strcpy(aux,options[i]);
                strcpy(options[i], options[j]);
                strcpy(options[j], aux);
            }
        }
    }

}

int checkRepeatedElements(char* options[], int size) {

    char last[15];

    sort(options, size);

    for(int i=0; i<size; i++) {
        if(strcmp(options[i], last) != 0) {
            strcpy(last, options[i]);
        }else {
            return true;
        }
    }
    return false;
}

int checkPresenceOfOption(char option[], char* options[]) {

    int i=0;

    while(*options[i] != '\0') {
        if(strcmp(options[i],option) == 0) {
            return true;
        }
        i++;
    }
    return false;
}
