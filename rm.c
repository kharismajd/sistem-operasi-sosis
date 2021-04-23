void main() {
    char path[64]; 
    char fileName[14];
    char fileName2[14];
    char currDir;
    char dirNfiles[512*2];
    char buffer[512];
    char dirIdx;
    char dirResult;
    int i;
    int j;
    int ketemu;
    int idxNama;
    int isNamaSama;
    int dirNfilesentry;
    int sukses;
    ketemu = 0;
    
    clear(path, 64);
    clear(fileName, 14);
    clear(fileName2, 14);

    readSector(dirNfiles, 0x101);
    readSector(dirNfiles + 512, 0x102);
    readSector(buffer, 511);
    currDir = buffer[0];

    for (i = 4; buffer[i] != 0x0; i++) {
		path[i - 4] = buffer[i];
	}

    ketemu = 0;
    getDirIdxFromPath(path, currDir, &dirIdx, &dirResult);
    getFileNameFromPath(path, fileName);
    for (i = 0; i < 64; i++) {
        if (dirNfiles[i * 16] == dirIdx) {
            for (j = 0; j < 14; j++) {
                fileName2[j] = dirNfiles[16 * i + 2 + j];
            }
            if (strCompare(fileName, fileName2, 14)) {
                ketemu = 1;
                break;
            }
        }
    }

    /*
    i = 0;
    while (!ketemu && i < 1024) { // Telusuri sampai sektor file terakhir
        for (i = 0; i < 1024; i += 16) {
            if (dirNfiles[i] == currDir) {
                if (dirNfiles[i+2] != 0x0) {
                    idxNama = i + 2;

                    // Samakan nama
                    isNamaSama = 1;
                    for (j = 0; j < 14; j++) {
                        if (nama[j] != dirNfiles[j + idxNama]) {
                            isNamaSama = 0;
                            break;
                        }
                        else if (dirNfiles[j + idxNama] == '\0' && nama[j] == '\0') {
                            break;    
                        }
                    }
                }

                if (isNamaSama) {
                    ketemu = 1;
                    dirNfilesentry = dirNfiles[i+1];
                    break;
                }
            }
        }
    }*/

    if (!ketemu) {
        printString("File/folder tidak ditemukan\r\n");
    }
    else {
        if (dirNfiles[i * 16 + 1] == 0xFF) {
            delDir(i);
            printString("Folder berhasil dihapus!\r\n");
        }
        else {
            delFile(i);
            printString("File berhasil dihapus!\r\n");
        }
    }
    executeProgram("shell", 0x2000, &sukses, 0x00);
}