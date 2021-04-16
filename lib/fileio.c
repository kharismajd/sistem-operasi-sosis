#include "fileio.h"

void readSector(char *buffer, int sector) {
	interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector) {
	interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
	char map[512];
	char files[1024];
	char sector[512];
	char fileName[14];
	char dirIdx;

	int emptyFilesIdx;
	int emptySectorIdx;
	int emptyMapIdx;
	int fileExist;
	int fileIndex;
	int mapIdx;
	int sectorIdx;
	int freeSectorCount;
	int sectorNeededCount;
	int bufferIdx;
	int dummyFileIdx;
	int dirValid;
	int i;

	readSector(map, 0x100);
	readSector(files, 0x101);
	readSector(files + 512, 0x102);
	readSector(sector, 0x103);

	emptyFilesIdx = -1;
	for (fileIndex = 0; fileIndex < 64; fileIndex++)
	{
		if (files[fileIndex * 16] == 0x0 && files[fileIndex * 16 + 1] == 0x0 && files[fileIndex * 16 + 2] == 0x0) {
			emptyFilesIdx = fileIndex;
			break;
		}
	}

	if (emptyFilesIdx == -1) {
		printString("Tidak cukup entri di files\r\n");
		*sectors = -2;
		return;
	}

	freeSectorCount = 0;
	for (mapIdx = 0; mapIdx < 512; mapIdx++) {
		if (map[mapIdx] == 0x0) {
			freeSectorCount += 1;
		}
	}
	
	bufferIdx = 0;
	sectorNeededCount = 0;
	while (buffer[bufferIdx] != 0x0) {
		bufferIdx++;
	}
	sectorNeededCount = 1 + div(bufferIdx, 512);

	if (sectorNeededCount > freeSectorCount) {
		printString("Tidak cukup sektor kosong\r\n");
		*sectors = -3;
		return;
	}

	emptySectorIdx = -1;
	for (sectorIdx = 0; sectorIdx < 32; sectorIdx++)
	{
		if (sector[sectorIdx * 16] == 0x0) {
			emptySectorIdx = sectorIdx;
			break;
		}
	}
	
	if (emptySectorIdx == -1) {
		printString("Tidak cukup sektor kosong\r\n");
		*sectors = -3;
		return;
	}

	searchFile(path, parentIndex, &dummyFileIdx, &fileExist);
	if (fileExist) {
		printString("File sudah ada\r\n");
		*sectors = -1;
		return;
	}

	getDirIdxFromPath(path, parentIndex, &dirIdx, &dirValid);
	if (!dirValid) {
		printString("Folder tidak valid\r\n");
		*sectors = -4;
		return;
	}

	clear(files + emptyFilesIdx * 16, 16);
	files[emptyFilesIdx * 16] = dirIdx;
	files[emptyFilesIdx * 16 + 1] = emptySectorIdx;
	getFileNameFromPath(path, fileName);
	for (i = 0; i < 14; i++)
	{
		files[emptyFilesIdx * 16 + 2 + i] = fileName[i];
	}

	for (i = 0; i < sectorNeededCount; i++)
	{
		emptyMapIdx = 0;
		for (mapIdx = 0; mapIdx < 512; mapIdx++)
		{
			if (map[mapIdx] == 0x0) {
				emptyMapIdx = mapIdx;
				map[mapIdx] = 0xFF;
				break;
			}
		}
		sector[emptySectorIdx * 16 + i] = emptyMapIdx;
		writeSector(buffer + (i * 512), emptyMapIdx);
	}
	
	*sectors = 0;
	writeSector(map, 0x100);
	writeSector(files, 0x101);
	writeSector(files + 512, 0x102);
	writeSector(sector, 0x103);
}

void readFile(char *buffer, char *path, int *result, char parentIndex) {
	char map[512];
	char files[1024];
	char sectors[512];
	int fileExist;
	int fileIndex;
	int i, sectorIdx;

	readSector(map, 0x100);
	readSector(files, 0x101);
	readSector(files + 512, 0x102);
	readSector(sectors, 0x103);

	searchFile(path, parentIndex, &fileIndex, &fileExist);

	if (fileExist) {
		*result = 0;
		sectorIdx = files[fileIndex * 16 + 1];
		for (i = 0; i < 16; i++)
		{
			if (sectors[sectorIdx * 16 + i] != 0x0) {
				readSector(buffer + (i * 512), sectors[sectorIdx * 16 + i]);
			}
			else {
				break;
			}
		}
	}
	else {
		printString("File tidak ditemukan\r\n");
		*result = -1;
	}
}

void searchFile(char *path, char parentIndex, char *index, char *result) {
	char files[1024];
	char fileName[14];
	char dirIdx;
	int dirResult;
	int fileIndex;
	
	clear(files, 1024);
	clear(fileName, 14);

	readSector(files, 0x101);
	readSector(files + 512, 0x102);

	getDirIdxFromPath(path, parentIndex, &dirIdx, &dirResult);
	getFileNameFromPath(path, fileName);

	*result = 0;
	*index = 0;
	if (dirResult == 1) {
		for (fileIndex = 0; fileIndex < 64; fileIndex++)
		{
			if (files[fileIndex * 16] == dirIdx && files[fileIndex * 16 + 1] != 0xFF && strCompare(fileName, files + fileIndex * 16 + 2, 14)) {
				*index = fileIndex;
				*result = 1;
				break;
			}
		}
	}
}

void getFileNameFromPath(char *path, char *fileName) {
	int i, j;

	j = 0;
	for (i = 0; path[i] != 0x0; i++) {
		if (path[i] != '/') {
			fileName[j] = path[i];
			j++;
		}
		else {
			j = 0;
		}
	}

	for (; j < 14; j++) {
		fileName[j] = 0x0;
	}
}

void getDirIdxFromPath(char *path, char parentIndex, char *dirIndex, int *result) {
	char files[1024];
	char dirName[14];
	int fileIndex;
	int found;
	int i, j;

	clear(files, 1024);
	clear(dirName, 14);

	readSector(files, 0x101);
	readSector(files + 512, 0x102);

	*result = 1;
	*dirIndex = parentIndex;
	i = 0;
	j = 0;
	if (path[0] == '/') {
		i += 1;
		*dirIndex = 0xFF;
	}
	
	for (; path[i] != 0x0; i++) {
    	if (path[i] != '/'){
    		dirName[j] = path[i];
			j++;
    	} 
		else {
    		for (; j < 14; j++) {
        		dirName[j] = 0x0;
      		}
			
			found = 0;
			if (dirName[0] == '.' && dirName[1] == 0x0) {
				found = 1;
			}
			else if (dirName[0] == '.' && dirName[1] == '.' && dirName[2] == 0x0) {
				if (*dirIndex != 0xFF) {
					*dirIndex = files[*dirIndex * 16];
					found = 1;
				}
			}
			else {
				for (fileIndex = 0; fileIndex < 64; fileIndex++) {
					if (files[fileIndex * 16] == *dirIndex && files[fileIndex * 16 + 1] == 0xFF && strCompare(dirName, files + fileIndex * 16 + 2, 14)) {
						*dirIndex = fileIndex;
						found = 1;
						break;
					}
				}
			}

			if (found) {
				j = 0;
			}
			else {
				*result = 0;
				break;
			}	
    	}
  	}
}

void delFile(char path) {
    char mapBuffer[512], folderAndFiles[1024], sectBuffer[512];
    int i;

    // Readsector yang terujung (257 & 258)
    interrupt(0x21, 0x02, folderAndFiles, 257, 0);
    interrupt(0x21, 0x02, folderAndFiles + 512, 258, 0);

    // Hapus path file
    folderAndFiles[path*16] = 0x0;
    folderAndFiles[path*16 + 1] = '\0';

    // Penghapusan pada map dan sektor
    interrupt(0x21, 0x02, &mapBuffer, 256, 0);
    interrupt(0x21, 0x02, &sectBuffer, 259, 0);
    i = 0;
    while (sectBuffer[path*16+1] != '\0' && i < 16) {
        // Kosongkan
        mapBuffer[sectBuffer[path*16+1]] = 0x0;
        sectBuffer[path*16+i] = 0x0;
        i++;
    }

    // Tulis kembali buffer ke sektors
    interrupt(0x21,0x03,&folderAndFiles,257,0);
    interrupt(0x21,0x03,folderAndFiles+512,258,0);
    interrupt(0x21,0x03,&sectBuffer,259,0);
    interrupt(0x21,0x03,&mapBuffer,256,0);
}