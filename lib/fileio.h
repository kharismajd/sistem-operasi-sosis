#ifndef FILEIO_H
#define FILEIO_H

void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void getDirIdxFromPath(char *path, char parentIndex, char *dirIndex, int *result);
void getFileNameFromPath(char *path, char *fileName);
void searchFile(char *path, char parentIndex, char *index, char *result);

#endif