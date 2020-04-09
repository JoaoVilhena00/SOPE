#ifndef DIRECTORY_H_
#define DIRECTORY_H_

#define DEV_BSIZE 512

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define true 1
#define false 0

void printDir(char *dirName);
void listContentsPrint(char *dirName, struct dirent *dentry, struct stat stat_entry, int hasb, int b_size);
int list_contents(char *dirName, char *options[], int b_size, int m_depth, char *argv[], int argc);


#endif // DIRECTORY_H_
