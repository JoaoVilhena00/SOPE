#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <ctype.h>
#include "auxiliary.h"
#include "directory.h"

#define true 1
#define false 0

void printUsage(char *argv[]) {
  fprintf(stderr, "Usage: %s -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n", argv[0]);
}

void invalidArgs(char *argv[]) {
  fprintf(stderr, "Invalid Arguments!\n");
  printUsage(argv);
  exit(1);
}

void invalidOption(char *argv[], char *option, int r) {
  fprintf(stderr, "%s is not a valid option!\n", option);
  printUsage(argv);
  exit(r);
}

void printDir(char *dirName) {
  fprintf(stderr, "Directory: ./%s\n", dirName);
}


int main(int argc, char *argv[], char *envp[]) {

    char dirName[100], *options[8], maxDepth[15], blockSize[15], bSize[15], forCheck[25];
    int j = 0, l = 0;

    for(int i=0; i<8; i++) {
        *(options+i) = (char*) malloc(15*sizeof(char));
    }

    if (argc < 2 || (strcmp(argv[1], "-l") && strcmp(argv[1], "--cout-links"))) { //Verifica se o utilizador degita a opçao -l
        invalidArgs(argv);
    }

    if (argc == 2){
        //em desenvolvimento
    }
    for (int i = 2; i < argc; i++){
        if (argv[i][0] != '-'){ // Adiciona o diretorio introduzido pelo utilizador a variavel dirName
            strcpy(dirName, argv[i]);
            continue;
        }
        if (argv[i][0] == '-' && argv[i][1] != '-'){ //Adiciona ao options as opçoes do tipo -(nome)
            if(argv[i][1] == 'B') {
                buildOption(argv[i], options[j], forCheck);
                if(!validOption(forCheck)){
                    invalidOption(argv, argv[i], 1);
                }
            }
            else {
                buildOption(argv[i], options[j], forCheck);
                if(!validOption(forCheck)){
                    invalidOption(argv, argv[i], 2);
                }
            }
            j++;
        }
        else if (argv[i][0] == '-' && argv[i][1] == '-'){ //temporario ate arranjar soluçao melhor
            if (argv[i][2] == 'm') { //Adiciona ao options max-depth
                makeOptinsDiff(argv[i], options[j], forCheck);
                if (!validOption(forCheck)) {
                    invalidOption(argv, argv[i], 3);
                }
            }
            else if (argv[i][2] == 'b') { //Adiciona ao options block-size
                makeOptinsDiff(argv[i], options[j],forCheck);
                if (!validOption(forCheck)) {
                    invalidOption(argv, argv[i], 4);
                }
            }
            else { //Adiciona ao options opcoes do tipo --(nome) (em vez das opçoes do tipo -(nome))
                makeOptinsDiff(argv[i], options[j], forCheck);
                if (!validOption(forCheck)) {
                    invalidOption(argv, argv[i], 5);
                }
            }
            j++;
        }

    }
    /*for (int i = 0; i < argc - 3; i++) { //So para questoes de teste
        printf("%s-->%d\n", options[i], i);
    }*/

    if(checkRepeatedElements(options, argc-3) == true) {
        printf("There is repeated elements\n");
        exit(6);
    }

    list_contents(dirName, options);

    return 0;
}
