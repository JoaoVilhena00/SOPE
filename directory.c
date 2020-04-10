#include "directory.h"
#include "auxiliary.h"

void printDir(char *dirName) {
  fprintf(stderr, "Directory: ./%s\n", dirName);
}

void listContentsPrint(char *dirName, struct dirent *dentry, struct stat stat_entry, int hasb, int b_size) {
  if(strcmp(dentry->d_name, ".") == 0) {}
  else if(strcmp(dentry->d_name, "..") == 0) {}
  else
    if(hasb == true) {
        printf("%d\t%s/%-25s\n", (int)stat_entry.st_size, dirName, dentry->d_name);
    } else if (b_size != -1) {
      stat_entry.st_blksize = b_size;
        printf("%d\t%s/%-25s\n", (int)(stat_entry.st_blocks * DEV_BSIZE / b_size), dirName, dentry->d_name);
    } else {
        printf("%d\t%s/%-25s\n", (int)stat_entry.st_blocks / 2, dirName, dentry->d_name);
    }
}

//Lista os Ficheiros Regulares (para ja)
int list_contents(char *dirName, char *options[], int b_size, int m_depth, char *argv[], int argc) {
  struct dirent *dentry;
  struct stat stat_entry;
  DIR *dir;
  int main_dir_size = 0;
  int hasb;
  char name[100];
  char name2[200];
  pid_t pid, childpid;
  int status;
  char *args[8];
  int help;
  char *arg[]={"./simpledu", "-l", "folder1", NULL};
  char *arg2[]={"./simpledu", "-l", "folder1/folder2", NULL};

  for(int i = 0; i < argc + 1; i++) {
      *(args+i) = (char*) malloc(100*sizeof(char));
  }

  for(int i = 0; i < argc; i++) {
    strcpy(args[i], argv[i]);
  }

  args[argc] = (char *) NULL;

  if((dir = opendir(dirName)) == NULL) {
      perror("Directory Error");
  }

  /*char cwd[PATH_MAX];
   if (getcwd(cwd, sizeof(cwd)) != NULL) {
       printf("Current working dir: %s\n", cwd);
   } else {
       perror("getcwd() error");
       return 1;
   }*/

    while((dentry = readdir(dir)) != NULL) {
        sprintf(name,"%s/%s",dirName,dentry->d_name);
        stat(name, &stat_entry);

        if(checkPresenceOfOption("b",options) || checkPresenceOfOption("bytes",options)) {
            hasb = true;
        }else {
            hasb = false;
        }

        if(hasb == true && strcmp(dentry->d_name, dirName)) {
          main_dir_size += (int)stat_entry.st_size;
        } else if(hasb == false)
            main_dir_size += (int)stat_entry.st_blocks / 2;

        if(S_ISDIR(stat_entry.st_mode) && strcmp(dentry->d_name,".") && strcmp(dentry->d_name,"..")) {
          strcpy(name, dirName);
          strcat(name, "/");
          strcat(name, dentry->d_name);
          strcpy(args[2], name);

          pid = fork();
          if(pid > 0) {
            childpid = wait(&status);
          } else if(pid == 0) {
            printf("EXECV: %d\n", execvp(args[0], args));
            perror("EXEC ERROR");
          }
        }

        if((checkPresenceOfOption("L", options)
            || checkPresenceOfOption("dereference", options)) && (checkPresenceOfOption("a", options)
            || checkPresenceOfOption("all", options))) {
            if (S_ISREG(stat_entry.st_mode)) {
              listContentsPrint(dirName, dentry, stat_entry, hasb, b_size);
            } else if (S_ISDIR(stat_entry.st_mode) && strcmp(dentry->d_name,".") && strcmp(dentry->d_name,"..")) {
              //listContentsPrint(dirName, dentry, stat_entry, hasb, b_size);
            }
        } else if (checkPresenceOfOption("a", options) || checkPresenceOfOption("all", options)){
            lstat(dentry->d_name, &stat_entry);
            if (S_ISREG(stat_entry.st_mode)) {
              listContentsPrint(dirName, dentry, stat_entry, hasb, b_size);
            } else if (S_ISDIR(stat_entry.st_mode) && strcmp(dentry->d_name,".") && strcmp(dentry->d_name,"..")) {
              //listContentsPrint(dirName, dentry, stat_entry, hasb, b_size);
            }else if(S_ISLNK(stat_entry.st_mode)) {
              listContentsPrint(dirName, dentry, stat_entry, hasb, b_size);
            }
        } else {
            if(S_ISDIR(stat_entry.st_mode) && strcmp(dentry->d_name,".") && strcmp(dentry->d_name,"..")) {
                //listContentsPrint(dirName, dentry, stat_entry, hasb, b_size);
            }
        }
    }

    printf("%d\t%s\n", main_dir_size, dirName);

    closedir(dir);

    exit(0);
}
