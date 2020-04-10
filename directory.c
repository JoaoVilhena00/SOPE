#include "directory.h"
#include "auxiliary.h"

void printDir(char *dirName)
{
  fprintf(stderr, "Directory: ./%s\n", dirName);
}

void listContentsPrint(char *dirName, struct dirent *dentry, struct stat stat_entry, int hasb, int b_size) {
  
  if (hasb == true) {
    printf("%d\t%s/%-25s\n", (int)stat_entry.st_size, dirName, dentry->d_name);
  }
  else if (b_size != -1) {
    stat_entry.st_blksize = b_size;
    printf("%d\t%s/%-25s\n", (int)(stat_entry.st_blocks * DEV_BSIZE / b_size), dirName, dentry->d_name);
  }
  else {
    printf("%d\t%s/%-25s\n", (int)stat_entry.st_blocks / 2, dirName, dentry->d_name);
  }
  

}

int funcToFork(char *dirName, char *options[], int m_depth, int argc, char *args[], struct dirent *dentry) {

  int status;
  pid_t pid, childpid;
  char name[100];
  
  strcpy(name, dirName);
  strcat(name, "/");
  strcat(name, dentry->d_name);
  strcpy(args[2], name);
  
  if (checkPresenceOfOption("L", options) || checkPresenceOfOption("dereference", options)) {
    m_depth = m_depth - 1;
    sscanf(args[argc - 1], "--max-depth=%d", &m_depth);
  }
  pid = fork();
  if (pid == 0) {
    execv(args[0], &args[0]);
  }else if(pid < 0) {
    perror("Fork Error");
  }
  
  return pid;
}

//Lista os Ficheiros Regulares (para ja)
int list_contents(char *dirName, char *options[], int b_size, int m_depth, char *argv[], int argc) {

  struct dirent *dentry;
  struct stat stat_entry;
  DIR *dir;
  int main_dir_size = 0;
  int hasb;
  char *args[8];
  int childpid, status;

  for(int i=0; i<argc; i++) {
    *(args+i) = (char*) malloc (50*sizeof(char));
  }

  for (int i = 0; i < argc; i++){
    strcpy(args[i], argv[i]);
  }

  if ((dir = opendir(dirName)) == NULL){
    perror("Directory Error");
  }

  chdir(dirName);

  while ((dentry = readdir(dir)) != NULL){
    stat(dentry->d_name, &stat_entry);

    if (checkPresenceOfOption("b", options) || checkPresenceOfOption("bytes", options)){
      hasb = true;
    }
    else{
      hasb = false;
    }
    
    if(hasb == true && strcmp(dentry->d_name, dirName)) {
      main_dir_size += (int)stat_entry.st_size;
    }
    else if (hasb == false) {
      main_dir_size += (int)stat_entry.st_blocks / 2;
    }

    if ((checkPresenceOfOption("L", options) || checkPresenceOfOption("dereference", options)) 
        && (checkPresenceOfOption("a", options) || checkPresenceOfOption("all", options))) {
      if (S_ISREG(stat_entry.st_mode)){
        listContentsPrint(dirName, dentry, stat_entry, hasb, b_size);
      }
      else if (S_ISDIR(stat_entry.st_mode)){
        if(strcmp(".",dentry->d_name) && strcmp("..",dentry->d_name)) {
          printf("====>%s<====\n",dentry->d_name);
          childpid = funcToFork(dirName, options, m_depth, argc, args, dentry);
        }
      }
    }
    else if (checkPresenceOfOption("a", options) || checkPresenceOfOption("all", options)){
      lstat(dentry->d_name, &stat_entry);
      if (S_ISREG(stat_entry.st_mode)){
        listContentsPrint(dirName, dentry, stat_entry, hasb, b_size);
      }
      else if (S_ISDIR(stat_entry.st_mode)) {
        if(strcmp(".",dentry->d_name) && strcmp("..",dentry->d_name)) {
          printf("====>%s<====\n",dentry->d_name);
          childpid = funcToFork(dirName, options, m_depth, argc, args, dentry);
        }
      }
      else if (S_ISLNK(stat_entry.st_mode)){
        listContentsPrint(dirName, dentry, stat_entry, hasb, b_size);
      }
    }
    else{
      if (S_ISDIR(stat_entry.st_mode)){
        if(strcmp(".",dentry->d_name) && strcmp("..",dentry->d_name)) {
          printf("====>%s<====\n",dentry->d_name);
          childpid = funcToFork(dirName, options, m_depth, argc, args, dentry);
        }
      }
    }
  }
  printf("%d\t%s\n", main_dir_size, dirName);
  
  
  while((waitpid(-1, &status,WNOHANG) != -1)) {
  }
  return 0;
}



