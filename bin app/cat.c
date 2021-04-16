int main()
{
    char buffer[512];
    char arg[512];
    char files[1024];
    char folderName[14];

    char dirIndex;
    char fileIndex;
    char currDir;

    int dirValid;
    int argIdx;
    int i;

    readSector(buffer, 0x0F);

    currDir = buffer[0];
    argIdx = 0;
    if (buffer[5] != 0x0) {
		for (i = 5; buffer[i] != 0x0; i++) {
			arg[argIdx] = buffer[i];
			argIdx++;
		}
		
        readSector(files, 0x101);
        readSector(files + 512, 0x102);

        getDirIdxFromPath(arg, currDir, &dirIndex, &dirValid);
        getFileNameFromPath(arg, folderName);
        if (folderName[0] == 0x0) {
            currDir = dirIndex;
            return;
        }
        else {
            for (fileIndex = 0; fileIndex < 64; fileIndex++) {
                if (files[fileIndex * 16] == dirIndex && files[fileIndex * 16 + 1] == 0xFF && strCompare(folderName, files + fileIndex * 16 + 2, 14)) {
                    currDir = fileIndex;
                    return;
                }
            }
            printString(folderName);
            printString(" bukan sebuah folder/tidak ada pada direktori\r\n");
        }
	}
}