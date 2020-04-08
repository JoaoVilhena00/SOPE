#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include "auxiliary.h"
#include "directory.h"

#define true 1
#define false 0
int logfile;//fazer LOG_FILENAME="../../logfile"

int main(int argc, char *argv[], char *envp[]) {
    logfile = open(getenv("LOG_FILENAME"), O_RDWR | O_CREAT | O_TRUNC, 0666);
    char dirName[100], *options[8], forCheck[25];
    int j = 0, l = 0, aux;
    int b_size = -1, m_depth = -1;
    long conv;
    char *key, *p;

    for(int i=0; i<8; i++) {
        *(options+i) = (char*) malloc(15*sizeof(char));
    }

    if (argc < 2 || (strcmp(argv[1], "-l") && strcmp(argv[1], "--count-links"))) { //Verifica se o utilizador degita a opçao -l
        invalidArgs(argv);
    }

    for (int i = 2; i < argc; i++){
        printf("%s\n", argv[i]);
        if (argv[i][0] != '-'){ // Adiciona o diretorio introduzido pelo utilizador a variavel dirName
            strcpy(dirName, argv[i]);
            continue;
        }
        else if (argv[i][0] == '-' && argv[i][1] != '-'){ //Adiciona ao options as opçoes do tipo -(nome)
            if(argv[i][1] == 'B') {
                buildOption(argv[i], options[j], forCheck);
                if(!validOption(forCheck)){
                    invalidOption(argv, argv[i], 1);
                }
                
                key = strtok(argv[i]," ");
                if((m_depth = atoi(strtok(NULL," "))) == 0) {
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
                key = strtok(argv[i],"=");
                if((m_depth = atoi(strtok(NULL,"="))) == 0) {
                    invalidOption(argv, argv[i], 3);
                }
            }  
            else if (argv[i][2] == 'b') { //Adiciona ao options block-size
                makeOptinsDiff(argv[i], options[j],forCheck);
                if (!validOption(forCheck)) {
                    invalidOption(argv, argv[i], 4);
                }
                key = strtok(argv[i],"=");
                if((b_size = atoi(strtok(NULL,"="))) == 0) {
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
        }else 
            invalidOption(argv,argv[i], 6); j++;
        
    
    }

    if (b_size == -1) {
      if(checkRepeatedElements(options, argc-3) == true) {
          printf("There is repeated elements\n");
          exit(6);
      }
    } else {
      if(checkRepeatedElements(options, argc-4) == true) {
          printf("There is repeated elements\n");
          exit(6);
      }
    }
    
    printf("%d\n",b_size);

    list_contents(dirName, options, b_size, m_depth);

    return 0;
}
