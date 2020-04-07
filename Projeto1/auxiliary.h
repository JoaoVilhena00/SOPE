#ifndef AUXILIARY_H_
#define AUXILIARY_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0

void printUsage(char *argv[]);
void invalidArgs(char *argv[]);
void invalidBArg(char *argv[], char *val);
void invalidOption(char *argv[], char *option, int r);

void makeOptinsDiff(char *option, char *specialOption, char *check);
void buildOption(char *argv, char *option, char *aux);
void sort(char *options[], int size);
int validOption(char *option);
int checkRepeatedElements(char* options[], int size);
int checkPresenceOfOption(char option[], char* options[]);

#endif // AUXILIARY_H_
