#include "folderio.h"

int isFolder(char *path, char parentIndex) {
	char files[1024];
	char fileName[14];
	char dirIdx;
	char fileIdx;

	int dirValid;

	readSector(files, 0x101);
	readSector(files + 512, 0x102);

	getDirIdxFromPath(path, parentIndex, &dirIdx, &dirValid);
	if (dirValid) {
		getFileNameFromPath(path, fileName);
		for (fileIdx = 0; fileIdx < 64; fileIdx++) {
			if (files[fileIdx * 16] == dirIdx && files[fileIdx * 16 + 1] == 0xFF && strCompare(fileName, files + fileIdx * 16 + 2, 14)) {
				return 1;
			}
		}
	}
	return 0;
}

int delDir(char path) { // Kurang di ngubah map.img & sectors.img
// kalo ada yg hardlink jangan sampai sectors sama mapnya ilang, folder doang aja
    char folderAndFiles[1024];
    int i;

    interrupt(0x21, 0x02, folderAndFiles, 257, 0);
    interrupt(0x21, 0x02, folderAndFiles + 512, 258, 0);

    // Menghapus entry folder
    folderAndFiles[path*16] = 0x0;
    folderAndFiles[path*16+1] = '\0';

    // Menulis kembali buffer ke sector
    interrupt(0x21, 0x03, &folderAndFiles, 257, 0);
    interrupt(0x21, 0x03, folderAndFiles + 512, 258, 0);

    // Hapus setiap file pada folder
    for (i = 0; i < 64; i++) {
        if (folderAndFiles[i*16] != 0xFF) { // Jika bukan 0xFF, maka bukan file
            if (folderAndFiles[i*16] == path && folderAndFiles[i*16+1] != '\0') { // Pada path & bukan end of line, artinya file
                hapusFile(i); // Hapus file pada folder path
            }
        }
    }

    // Hapus setiap folder pada folder
    for (i = 0; i < 64; i++) {
        if (folderAndFiles[i*16] == path && folderAndFiles[i*16+1] == 0xFF) { // Pada path & 0xFF pada ujung, artinya folder
            delDir(i); // Hapus folder pada folder path
        }
    }
    
}

void makeDir(char *path, char *parentIndex)
{
	char files[1024];
	char fileName[14];
	char dirIdx;
	int dirResult;
	int fileIndex;
	int i;

	readSector(files, 0x101);
	readSector(files + 512, 0x102);

	getDirIdxFromPath(path, parentIndex, &dirIdx, &dirResult);
	if (dirResult == 1)
	{
		getFileNameFromPath(path, fileName);
		for (fileIndex = 0; fileIndex < 64; fileIndex++)
		{
			if (files[fileIndex * 16] == 0x0 && files[fileIndex * 16 + 1] == 0x0 && files[fileIndex * 16 + 2] == 0x0)
			{
				files[fileIndex * 16] = dirIdx;
				files[fileIndex * 16 + 1] = 0xFF;
				for(i = 0; i < 14; i++)
				{
					files[fileIndex * 16 + 2 + 1] = fileName[i];
				}
			}
		}

		if (fileIndex == 64)
		{
			printString("Sektor file sudah penuh\r\n");
			return;
		}
		
		writeSector(files, 0x101);
		writeSector(files + 512, 0x102);
	}
	else
	{
		printString("Direktori invalid\r\n");
		return;
	}
}