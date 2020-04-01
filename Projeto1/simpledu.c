#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <ctype.h>

#define true 1
#define false 0

int validOption(char *option) {

    if (!strcmp(option, "a") || !strcmp(option, "b") || !strcmp(option, "B ")
        || !strcmp(option, "L") || !strcmp(option, "S") || !strcmp(option, "max-depth")
        || !strcmp(option, "block-size") || !strcmp(option, "all") || !strcmp(option, "bytes")
        || !strcmp(option, "dereference")
        || !strcmp(option, "separate-dirs")) {
        return true;
    }
    return false;
}

char*  makeOptinsDiff(char *option, char *specialOption) {

    int l = 0;
    
    while (option[l+2] != '='){
        specialOption[l] = option[l+2];
        l++;
    }
    specialOption[l] = '\0';
    
    return specialOption;
}

char* buildOption(char *argv, char *option, char * aux) {

    int l=0;

    while(argv[l+1] != '\0') {
        option[l] = argv[l+1];
        l++;
    }
    option[l] = '\0';
    
    aux[0] = option[0];
    aux[1] = option[1];
    aux[2] = '\0';
	
	return option;
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

//Lista os Ficheiros Regulares (para ja)
int list_contents(char *dirName, char *options[]) {
  struct dirent *dentry;
  struct stat stat_entry;
  DIR *dir;

  if((dir = opendir(dirName)) == NULL) {
      perror("Directory Error");
  }

  chdir(dirName);
  fprintf(stderr, "Files of ");
  printDir(dirName);

    while((dentry = readdir(dir)) != NULL) {
        lstat(dentry->d_name, &stat_entry); //com stat o symbolic link sem opçao -L era imprimido, nao sei a razão
        if (S_ISREG(stat_entry.st_mode)) {
            printf("%d\t%s/%-25s\n", (int)stat_entry.st_size, dirName, dentry->d_name);
        }
        if(S_ISLNK(stat_entry.st_mode)) { //Fazer assim nao sei se e melhor opçao(a unica que eu vi)
            if(checkPresenceOfOption("L", options) 
                || checkPresenceOfOption("deference", options)) {
                printf("%d\t%s/%-25s\n", (int)stat_entry.st_size, dirName, dentry->d_name);
            }
        }
    }
    return 0;
}
    
int main(int argc, char *argv[], char *envp[]) {

    char dirName[100], *options[8], maxDepth[15], blockSize[15], bSize[15], aux[3], forNow[15];
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
            printDir(dirName);
        }
        if (argv[i][0] == '-' && argv[i][1] != '-'){ //Adiciona ao options as opçoes do tipo -(nome)
            if(argv[i][1] == 'B') {
                strcpy(options[j],buildOption(argv[i], forNow, aux));
                if(!validOption(aux)){
                    invalidOption(argv, argv[i], 1);
                }
            }
            else {
                strcpy(options[j], buildOption(argv[i], forNow, aux));
                if(!validOption(options[j])){
                    invalidOption(argv, argv[i], 2);
                }
            }
            j++;
        }
        else if (argv[i][0] == '-' && argv[i][1] == '-'){ //temporario ate arranjar soluçao melhor
            if (argv[i][2] == 'm') { //Adiciona ao options max-depth
            strcpy(options[j], makeOptinsDiff(argv[i], maxDepth));
                printf("%s\n", options[j]);
                if (!validOption(maxDepth)) {
                    invalidOption(argv, argv[i], 3);
                }
            }
            else if (argv[i][2] == 'b') { //Adiciona ao options block-size
                strcpy(options[j],makeOptinsDiff(argv[i], blockSize));
                if (!validOption(blockSize)) {
                    invalidOption(argv, argv[i], 4);
                }
            }
            else { //Adiciona ao options opcoes do tipo --(nome) (em vez das opçoes do tipo -(nome))
                strcpy(options[j],makeOptinsDiff(argv[i], options[j]));
                if (!validOption(options[j])) {
                    invalidOption(argv, argv[i], 5);
                }
            }
            j++;
        }
       
    }
    for (int i = 0; i < argc - 3; i++) { //So para questoes de teste
        printf("%s-->%d\n", options[i], i);
    }
    
    if(checkRepeatedElements(options, argc-3) == true) {
        printf("There is repeated elements\n");
        exit(6);
    }
    

    list_contents(dirName, options);

    return 0;
}

