void main() {
    char path[64]; 
    char fileName[14];
    char currDir;
    char dirNfiles[512*2];
    char argv[4][64];
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
    readSector(buffer,512);
    currDir = buffer[0];

    // Dapatkan idx parent dan nama
    for (i = 0; i < 14; i++) {
        fileName[i] = buffer[i+1];
    }
    printString("\r\n\n");

    // Baca sektor files / folder
    readSector(dirNfiles, 0x101);
    readSector(dirNfiles + 512, 0x102);

    ketemu = 0;
    for (i = 0; i < 64; i++) {
        getDirIdxFromPath(path, currDir, &dirIdx, &dirResult);
        if (dirResult == 1) {
            getFileNameFromPath(path, fileName);
            if (dirNfiles[i * 16] == dirIdx && strCompare(fileName, dirNfiles + i * 16 + 2, 14)) {
                if (i == 8) {
                    printString("Testing");
                }
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