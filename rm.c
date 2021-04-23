void main() {
    char nama[14]; 
    char currDir;
    char dirNfiles[512*2];
    char direktori[512];
    int i;
    int j;
    int ketemu;
    int idxNama;
    int isNamaSama;
    int dirNfilesentry;
    int sukses;

    ketemu = 0;

    // Dapatkan idx parent dan nama
    interrupt(0x21, 0x02, direktori, 0x0D, 0);
    currDir = direktori[0];
    for (i = 0; i < 14; i++) {
        nama[i] = direktori[i+1];
    }

    // Baca sektor files & folder
    interrupt(0x21, 0x02, dirNfiles, 257, 0); // Karena 257 adalah sektor files yang pertama
    interrupt(0x21, 0x02, dirNfiles + 512, 258, 0); // Karena 258 adalah sektor files yang kedua sehingga harus diperiksa

    i = 0;
    while (!ketemu && i < 1024) { // Telusuri sampai sektor file terakhir
        for (i = 0; i < 1024; i += 16) {
            if (dirNfiles[i] == currDir && dirNfiles[i+2] != 0x0) { // Jika direktorinya sama...
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

                if (isNamaSama) {
                    ketemu = 1;
                    dirNfilesentry = dirNfiles[i+1];
                    break;
                }
            }
        }
    }
    if (!ketemu) {
        printString("File/folder tidak timukan\r\n");
    }
    else {
        if (dirNfilesentry == 0xFF) {
            delDir(dirNfilesentry);
            printString("Folder berhasil dihapus!\r\n");
        }
        else {
            delFile(dirNfilesentry);
            printString("File berhasil dihapus!\r\n");
        }
    }
    executeProgram("shell", 0x2000, &sukses, 0x00);
}