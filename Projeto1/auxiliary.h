#ifndef AUXILIARY_H_
#define AUXILIARY_H_


void makeOptinsDiff(char *option, char *specialOption, char *check);
void buildOption(char *argv, char *option, char *aux);
void sort(char *options[], int size);
int validOption(char *option);
int checkRepeatedElements(char* options[], int size);
int checkPresenceOfOption(char option[], char* options[]);

#endif // AUXILIARY_H_
