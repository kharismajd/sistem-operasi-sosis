#ifndef FOLDERIO_H
#define FOLDERIO_H

int isFolder(char *path, char parentIndex);
int delDir(char path);
void makeDir(char *path, char *parentIndex);
int searchPath(char* dirCall, int parentIndex);

#endif