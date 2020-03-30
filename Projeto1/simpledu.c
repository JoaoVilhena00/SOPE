#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#define true 1
#define false 0

int validOption(char *option) {

    if (!strcmp(option, "a") || !strcmp(option, "b") || !strcmp(option, "B")
        || !strcmp(option, "L") || !strcmp(option, "S") || !strcmp(option, "max-depth")
        || !strcmp(option, "block-size") || !strcmp(option, "all") || !strcmp(option, "bytes")
        || !strcmp(option, "count-links") || !strcmp(option, "dereference") || !strcmp(option, "dereference")
        || !strcmp(option, "separate-dirs")) {
        return true;
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
int list_contents(char *dirName) {
  struct dirent *dentry;
  struct stat stat_entry;
  DIR *dir;

  dir = opendir(dirName);

  chdir(dirName);
  fprintf(stderr, "Files of ");
  printDir(dirName);

  while((dentry = readdir(dir)) != NULL) {
    stat(dentry->d_name, &stat_entry);
    if (S_ISREG(stat_entry.st_mode)) {
      printf("%d\t%s/%-25s\n", (int)stat_entry.st_size, dirName, dentry->d_name);
    }
  }

  return 0;
}


int main(int argc, char *argv[], char *envp[]) {

    char dirName[100], *options[8], maxDepth[12], blockSize[20];
    int j = 0, l = 0;

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
        else if (argv[i][0] == '-' && argv[i][1] != '-'){ //Adiciona ao options as opçoes do tipo -(nome)
            if (!validOption(argv[1])){
                invalidOption(argv, options[j], 2);
            }
            *options[j] = argv[i][1];
            j++;
        }
        else if (argv[i][0] == '-' && argv[i][1] == '-'){ //temporario ate arranjar soluçao melhor
            l = 0;
            if (argv[i][2] == 'm') { //Adiciona ao options max-depth
                while (argv[i][l+2] != '='){
                    maxDepth[l] = argv[i][l+2];
                    l++;
                }
                maxDepth[l] = '\0';
                options[j] = maxDepth;
                l = 0;
                if (!validOption(maxDepth)) {
                    invalidOption(argv, options[j], 3);
                }
            }else if (argv[i][2] == 'b') { //Adiciona ao options block-size
                while (argv[i][l] != '=') {
                    blockSize[l] = argv[i][l+2];
                    l++;
                }
                blockSize[l] = '\0';
                options[j] = blockSize;
                if (!validOption(blockSize)) {
                    invalidOption(argv, options[j], 3);
                }
            }else { //Adiciona ao options opcoes do tipo --(nome) (em vez das opçoes do tipo -(nome))
                while(argv[i][l+2] != '\0') {
                    options[j][l] = argv[i][l+2];
                    l++;
                }
                options[j][l] = '\0';
                if (!validOption(options[j])) {
                    invalidOption(argv, options[j], 3);
                }
            }
            j++;
        }
    }

    for (int i = 0; i < argc - 3; i++) { //So para questoes de teste
        printf("%s %d\n", options[i], i);
    }

    list_contents(dirName);

    return 0;
}
