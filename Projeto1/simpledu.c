#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0

int validOption(char *option) {

    if(!strcmp(option, "-a") || !strcmp(option, "-b") || !strcmp(option, "-B") 
        || !strcmp(option, "-L") || !strcmp(option, "-S") || !strcmp(option, "-N")) {
            return true;
    }
    return false;
}


int main(int argc, char *argv[], char *envp[]) {

    char dirName[100], *options[8];
    int j = 0;

    
    if(argc < 2 || strcmp(argv[1],"-l")) {
        printf("Invalid Arguments!\n");
        exit(1);
    }

    if(argc == 2) {
        //em desenvolvimento
    }

    for(int i=2; i<argc; i++) {
        if(argv[i][0] != '-') {
            strcpy(dirName,argv[i]);
            printf("%s\n",dirName);
        }
        else {
            options[j] = argv[i];
            if(!validOption(options[j])) {
                printf("%s is a invalid option\n", options[j]);
                exit(2);
            }
            j++;
        }
    }
    
    for(int i=0; i<=argc-4; i++) { //So para questoes de teste
        printf("%s\n",options[i]);
    }

    return 0;

}