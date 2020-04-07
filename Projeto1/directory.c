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
        printf("%d\t%s/%-25s\n", (int)stat_entry.st_blocks, dirName, dentry->d_name);
    }
}

//Lista os Ficheiros Regulares (para ja)
int list_contents(char *dirName, char *options[], int b_size) {
  struct dirent *dentry;
  struct stat stat_entry;
  DIR *dir;
  int main_dir_size = 0;
  int hasb;

  printDir(dirName);
  if((dir = opendir(dirName)) == NULL) {
      perror("Directory Error");
  }

  chdir(dirName);

    while((dentry = readdir(dir)) != NULL) {
        stat(dentry->d_name, &stat_entry);
        if(checkPresenceOfOption("b",options) || checkPresenceOfOption("bytes",options)) {
            hasb = true;
        }else {
            hasb = false;
        }

        if (strcmp(dentry->d_name, ".") == 0){
            if(hasb == true)
                main_dir_size = (int)stat_entry.st_size;
            else
                main_dir_size = (int)stat_entry.st_blocks;
        }

        if((checkPresenceOfOption("L", options)
            || checkPresenceOfOption("dereference", options)) && (checkPresenceOfOption("a", options)
            || checkPresenceOfOption("all", options))) {
            if (S_ISREG(stat_entry.st_mode)) {
              listContentsPrint(dirName, dentry, stat_entry, hasb, b_size);
            } else if (S_ISDIR(stat_entry.st_mode)) {
              listContentsPrint(dirName, dentry, stat_entry, hasb, b_size);
            }  else if (S_ISLNK(stat_entry.st_mode)) {
              listContentsPrint(dirName, dentry, stat_entry, hasb, b_size);
            }
        } else if (checkPresenceOfOption("a", options) || checkPresenceOfOption("all", options)){
            if (S_ISREG(stat_entry.st_mode)) {
              listContentsPrint(dirName, dentry, stat_entry, hasb, b_size);
            } else if (S_ISDIR(stat_entry.st_mode)) {
              listContentsPrint(dirName, dentry, stat_entry, hasb, b_size);
            }
        } else {
            if(S_ISDIR(stat_entry.st_mode)) {
                listContentsPrint(dirName, dentry, stat_entry, hasb, b_size);
            }
        }
    }

    printf("%d\t%s\n", main_dir_size, dirName);

    return 0;
}
