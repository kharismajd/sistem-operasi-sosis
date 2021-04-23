void printShell(char parentIndex);
void cd(char *path, char *parentIndex);
void ls(char parentIndex);
int searchPath(char* dirCall, int parentIndex);
char currentDir;
char files[1024];

void main() {

	char tempBuff[512];
	char arg1[64];
	char arg2[64];
	char input[128];
	char fileName[14];
	char dirIdx;
    char fileIdx;
    char fileExist;

	int arg1Idx, arg2Idx;
	int i;
	int badCommand = 0;
	int success = 1;
	currentDir = 0xFF;


    while (1) {
        readSector(files, 0x101);
        readSector(files + 512, 0x102);

        printShell(currentDir);
        
        clear(input, 64);
        clear(tempBuff, 512);
        readString(input);		

        i = 0;
        badCommand = 0;

        clear(arg1, 64);
        clear(arg2, 64);
        clear(fileName, 14);
        arg1Idx = 0;
        arg2Idx = 0;

        if (strCompare(input, "cd ", 3)) {
            if (input[3] != 0x0) {
                for(i = 3; input[i] != 0x0; i++) {
                    arg1[arg1Idx] = input[i];
                    arg1Idx++;
                }
                cd(arg1, &currentDir);
            }
        }
        else if (strCompare(input, "ls", 2)) {
            ls(currentDir);
        }
        else if (strCompare(input,"./", 2)) {
            for (i = 2; input[i] != 0x0; i++)
            {
                arg1[i-2] = input[i];
            }
            getDirIdxFromPath(arg1, currentDir, &dirIdx, success);
            getFileNameFromPath(arg1, fileName);
            executeProgram(fileName, 0x3000, success, dirIdx);
        }
        else {
            tempBuff[0] = currentDir;
            for (i = 0; input[i] != ' '; i++)
            {
                if (input[i] == 0x0) {
                    break;
                }
                fileName[i] = input[i];
            }

            for (i = 1; input[i-1] != 0x0; i++)
            {
                tempBuff[i] = input[i-1];
            }

            searchFile(fileName, 0x00, &fileIdx, &fileExist);
            if (fileExist) {
                writeSector(tempBuff, 511);
                clear(tempBuff, 512);
                executeProgram(fileName, 0x3000, success, 0x00);
            }
            else {
                printString(fileName);
                printString(" tidak ada pada folder bin");
            }
        }
        printString("\r\n");
    }
}

void cd(char *path, char *parentIndex) {
	char files[1024];
	char folderName[14];

	char dirIndex;
	char fileIndex;
	int dirValid;

	readSector(files, 0x101);
	readSector(files + 512, 0x102);

	getDirIdxFromPath(path, *parentIndex, &dirIndex, &dirValid);
	getFileNameFromPath(path, folderName);
	if (folderName[0] == 0x0) {
		*parentIndex = dirIndex;
		return;
	}
	else {
		for (fileIndex = 0; fileIndex < 64; fileIndex++) {
			if (files[fileIndex * 16] == dirIndex && files[fileIndex * 16 + 1] == 0xFF && strCompare(folderName, files + fileIndex * 16 + 2, 14)) {
				*parentIndex = fileIndex;
				return;
			}
		}
		printString(folderName);
		printString(" bukan sebuah folder/tidak ada pada direktori\r\n");
	}
}

void ls(char parentIndex)
{
    char files[1024];
	char fileName[14];

	int fileIndex;
    int i;

	readSector(files, 0x101);
	readSector(files + 512, 0x102);

    for(fileIndex = 0; fileIndex < 64; fileIndex++)
    {     
        if (files[fileIndex * 16] == parentIndex && files[fileIndex * 16 + 2] != 0x0) {
            clear(fileName, 14);
            for(i = 0; i < 14; i++) {
                fileName[i] = files[fileIndex * 16 + 2 + i];
            }
            printString(fileName);
            printString("\r\n");
        }
    }
}

void printShell(char parentIndex) {
	char files[1024];
	char fileName[14];
	char stack[64];

	char dirIdx;
	char currIdx;

	int i, j;

	readSector(files, 0x101);
	readSector(files + 512, 0x102);

	dirIdx = parentIndex;
	clear(stack, 64);
	stack[0] = dirIdx;

	i = 0;
	while (dirIdx != 0xFF) {
		i++;
		dirIdx = files[dirIdx * 16];
		stack[i] = dirIdx;
	}

	while (i >= 0) {
		currIdx = stack[i];

		if (currIdx == 0xFF) {
			printString("Root");
		}
		else {
			clear(fileName, 14);
			for(j = 0; j < 14; j++) {
				fileName[j] = files[currIdx * 16 + 2 + j];
			}

			printString("\\");
			printString(fileName);
		}
		i--;
	}
	printString(">");
}