#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0

int validOption(char *option) {  

    if (!strcmp(option, "-a") || !strcmp(option, "-b") || !strcmp(option, "-B") 
        || !strcmp(option, "-L") || !strcmp(option, "-S") || !strcmp(option, "--max-depth") 
        || !strcmp(option, "--block-size") || !strcmp(option, "--all") || !strcmp(option, "--bytes") 
        || !strcmp(option, "--count-links") || !strcmp(option, "--dereference") || !strcmp(option, "--dereference") 
        || !strcmp(option, "--separate-dirs")) {
        return true;
    }
    return false;
}

int main(int argc, char *argv[], char *envp[]) {

    char dirName[100], *options[8], maxDepth[12], blockSize[20];
    int j = 0, l = 0;

    if (argc < 2 || strcmp(argv[1], "-l") || strcmp(argv[1], "--cout-links")) { //Verifica se o utilizador degita a opçao -l 
        printf("Invalid Arguments!\n");
        exit(1);
    }

    if (argc == 2){
        //em desenvolvimento
    }

    for (int i = 2; i < argc; i++){
        if (argv[i][0] != '-'){ // Adiciona o diretorio introduzido pelo utilizador a variavel dirName
            strcpy(dirName, argv[i]);
            printf("%s\n", dirName);
        }
        else if (argv[i][0] == '-' && argv[i][1] != '-'){ //Adiciona ao options as opçoes do tipo -(nome)
            options[j] = argv[i];
            if (!validOption(options[j])){
                printf("%s is a invalid option\n", options[j]);
                exit(2);
            }
            j++;
        }
        else if (argv[i][0] == '-' && argv[i][1] == '-'){ //temporario ate arranjar soluçao melhor
            if (argv[i][2] == 'm') { //Adiciona ao options max-depth
                while (argv[i][l] != '='){
                    maxDepth[l] = argv[i][l];
                    l++;
                }
                maxDepth[l] = '\0';
                options[j] = argv[i];
                l = 0;
                if (!validOption(maxDepth)) {
                    printf("%s is a invalid option\n", options[j]);
                    exit(3);
                }
            }else if (argv[i][2] == 'b') { //Adiciona ao options block-size
                while (argv[i][l] != '=') {
                    blockSize[l] = argv[i][l];
                    l++;
                }
                blockSize[l] = '\0';
                options[j] = argv[i];
                l = 0;
                if (!validOption(blockSize)) {
                    printf("%s is a invalid option\n", options[j]);
                    exit(3);
                }
            }else { //Adiciona ao options opcoes do tipo --(nome) (em vez das opçoes do tipo -(nome))
                options[j] = argv[i];
            }
            j++;
        }
    }

    for (int i = 0; i <= argc - 4; i++) { //So para questoes de teste
        printf("%s %d\n", options[i], i);
    }

    return 0;
}