#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int validOption(char options) {


}





int main(int argc, char *argv[], char *envp[]) {

    char path[100], *options[8];
    int j = 0;

    if(argc < 2 || argc > 10 || (argv[2] != "-l" || argv[2] != "- count-links")) {
        printf("Invalid Arguments!\n");
    }

    if(argc == 2) {
        //em desenvolvimento
    }

    for(int i=0; i<argc; i++) {
        if(argv[i][0] != '-') {
            strcpy(path,argv[i]);
            printf("%s\n",path);
        }
        else {
            options[j] = argv[i];
            j++;
        }


    }
    
    
    for(int i=0; i<=argc-4; i++) {
        printf("%s\n",options[i]);
    }

}