#ifndef DIRECTORY_H_
#define DIRECTORY_H_

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define true 1
#define false 0

void listContentsPrint(char *dirName, struct dirent *dentry, struct stat stat_entry, int hasb);
int list_contents(char *dirName, char *options[]);


#endif // DIRECTORIES_H_


