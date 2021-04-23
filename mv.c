//#include lib.asm
//#include 

void main() {
    char files[1024];
    char buffer[512];
    char path[64];
    char fileName[14];
	char fileName2[14];
    char argv[4][64];
    char currDir;
    char dirIdx;
	char newDirIdx;
 
    int dummyResult;
    int argc;
    int countFiles;
    int i;
	int j;
	int k;
    int nameLength;
	int sudahAda;
 
    clear(files, 1024);
    clear(buffer, 512);
    clear(path, 64);
    clear(fileName, 14);
	clear(fileName2, 14);
    
    for (i = 0; i < 4; i++) {
        clear(argv[i], 64);
    }
    readSector(buffer, 511);
	readSector(files, 0x101);
	readSector(files + 512, 0x102);
 
    currDir = buffer[0];
 
    //Baca argumen
    i = 4;
    argc = 0;
    nameLength = 0;
    while (buffer[i] != 0x0)
    {
        if (buffer[i] == 32)
        {
            nameLength = 0;
            argc += 1;
            i += 1;
        }
 
        argv[argc][nameLength] = buffer[i];
        nameLength += 1;
        i += 1;
    }
    argc += 1;
 
    if (argc < 2)
    {
        printString("Cara menggunakan: mv <file/folder> <folder>\r\n");
        printString("Cara menggunakan: mv <file/folder 1> <file/folder 2> <file/folder n> <folder destinasi>\r\n");
        executeProgram("shell", 0x2000, &dummyResult, 0x00);
    }

	clear(path, 64);
	countFiles = argc - 1;
    strcpy(argv[argc - 1], path, 64);
    if (isFolder(path, currDir))
    {
		getDirIdxFromPath(path, currDir, &newDirIdx, &dummyResult);
		getFileNameFromPath(path, fileName);

		for (i = 0; i < 64; i++)
		{
			if (files[16 * i] == newDirIdx)
			{
				for (k = 0; k < 14; k++)
				{
					fileName2[k] = files[16 * i + 2 + k];
				}
				if (strCompare(fileName, fileName2, 14))
				{
					newDirIdx = i;
					break;
				}
			}
		}
        for (i = 0; i < countFiles; i++)
        {
            clear(path, 64);
            strcpy(argv[i], path, 64);

			getDirIdxFromPath(path, currDir, &dirIdx, &dummyResult);
			getFileNameFromPath(path, fileName);
			if (dummyResult == 1) {
				sudahAda = 0;
				for (j = 0; j < 64; j++)
				{
					if (files[16 * j] == newDirIdx)
					{
						for (k = 0; k < 14; k++)
						{
							fileName2[k] = files[16 * j + 2 + k];
						}
						if (strCompare(fileName, fileName2, 14))
						{
							printString(fileName);
							sudahAda = 1;
							break;
						}
					}
				}
				if (!sudahAda)
				{
					for (j = 0; j < 64; j++)
					{
						if (files[16 * j] == dirIdx)
						{
							for (k = 0; k < 14; k++)
							{
								fileName2[k] = files[16 * j + 2 + k];
							}
							if (strCompare(fileName, fileName2, 14))
							{
								files[16 * j] = newDirIdx;
								break;
							}
						}
					}
				}
				else
				{
					printString(fileName);
					printString(" sudah ada pada destinasi\r\n");
				}
			}     
        }
    }
    else
    {
        printString("Folder destinasi tidak ada\r\n");
    }

	writeSector(files, 0x101);
	writeSector(files + 512, 0x102);
    executeProgram("shell", 0x2000, &dummyResult, 0x00);
}