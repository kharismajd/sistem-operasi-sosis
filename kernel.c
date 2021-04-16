#include "lib/fileio.h"
#include "lib/math.c"
#include "lib/text.c"
#include "lib/folderio.c"

/* Ini deklarasi fungsi */
void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printLogo2(int baris, int kolom, char* s);
void printLogo();
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0
void cd(char *path, char *parentIndex);
void ls(char parentIndex);
void cat(char *path, char parentIndex);
void ln(char *fromPath, char *toPath, char parentIndex);
void executeProgram(char *filename, int segment, int *success, char parentIndex);

void printShell(char parentIndex);

int currentDir;
char files[1024];

int main() {
	char tempBuff[512];
	char arg1[64];
	char arg2[64];
	char input[128];
	int arg1Idx, arg2Idx;
	int i;
	int badCommand = 0;
	int success = 1;
	currentDir = 0xFF;

	printLogo();

	printString("\n\n\n\n");
	while(1){
		readSector(files, 0x101);
		readSector(files + 512, 0x102);

		printShell(currentDir);
		
		clear(input, 64);
		readString(input);		

		i = 0;
		badCommand = 0;

		clear(arg1, 32);
		clear(arg2, 32);
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
			tempBuff[0] = currentDir;
			for (i = 1; input[i] != 0x0; i++)
			{
				tempBuff[i] = input[i-1];
			}
			writeSector(tempBuff, 0x0F);
			clear(tempBuff, 512);
			executeProgram("ls", 0x2000, success, 0x00);
		}
		else if (strCompare(input, "cat ", 4)) {
			tempBuff[0] = currentDir;
			for (i = 1; input[i] != 0x0; i++)
			{
				tempBuff[i] = input[i-1];
			}
			writeSector(tempBuff, 0x0F);
			clear(tempBuff, 512);
			executeProgram("cat", 0x2000, success, 0x00);
		}
		else if (strCompare(input, "ln ", 3)) {
			tempBuff[0] = currentDir;
			for (i = 1; input[i] != 0x0; i++)
			{
				tempBuff[i] = input[i-1];
			}
			writeSector(tempBuff, 0x0F);
			clear(tempBuff, 512);
			executeProgram("ln", 0x2000, success, 0x00);
		}
		else if (strCompare(input,"mkdir ", 6)) {
			for (i = 6; input[i] != ' '; i++) {
				arg1[arg1Idx] = input[i];
				arg1Idx++;
				makeDir(arg1,currentDir);
			}
		}
		printString("\r\n");
	}

	return 0;
}

void handleInterrupt21 (int AX, int BX, int CX, int DX) { 
	char AL, AH; 
	AL = (char) (AX); 
	AH = (char) (AX >> 8); 
	switch (AL) { 
		case 0x00: 
			printString(BX); 
			break; 
		case 0x01: 
			readString(BX); 
			break; 
		case 0x02: 
			readSector(BX, CX); 
			break; 
		case 0x03: 
			writeSector(BX, CX); 
			break; 
		case 0x04: 
			readFile(BX, CX, DX, AH); 
			break; 
		case 0x05: 
			writeFile(BX, CX, DX, AH); 
			break;
		case 0x6:
		    executeProgram(BX, CX, DX, AH);
		    break;
		default: 
			printString("Invalid interrupt"); 
	} 
}

void printLogo2(int baris, int kolom, char* s){
   int i = 0;
   int offset=0x8000+((baris-1)*80*2)+(40-kolom/2)*2;
   while(s[i] != '\0'){
      putInMemory(0xB000, offset + i*2, s[i]);
      putInMemory(0xB000, offset + i*2+1, 0xD);
      i++;
   }
}
void printLogo() {
   printLogo2(13, 22,  "------------");
   printLogo2(14, 22, "|sOSissOSis|");
   printLogo2(15, 22, "|sissOSisso|");
   printLogo2(16, 22, "|issOSissOS|");
   printLogo2(17, 22, "|ssOSissOSi|");
   printLogo2(18, 22, "------------");
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

void ls(char parentIndex) {
	char files[1024];
	char fileName[14];

	int fileIndex, i;

	readSector(files, 0x101);
	readSector(files + 512, 0x102);

	for(fileIndex = 0; fileIndex < 64; fileIndex++) {
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

void cat(char *path, char parentIndex) {
	char buffer[8192];
	char fileName[14];
	int result;

	if (isFolder(path, parentIndex)) {
		getFileNameFromPath(path, fileName);
		printString(fileName);
		printString(" adalah sebuah folder\r\n");
		return;
	}

	readFile(buffer, path, &result, parentIndex);

	if (result == 0) {
		printString(buffer);
		printString("\r\n");
	}
}

void ln(char *fromPath, char *toPath, char parentIndex) {
	char files[1024];
	char fileName[14];

	char fileIndex;
	char dummyFileIndex;
	char emptyFilesIndex;
	char dirIndex;
	char fileSector;
	
	int fromFileExist;
	int toFileExist;
	int dirValid;
	int i;

	readSector(files, 0x101);
	readSector(files + 512, 0x102);

	searchFile(toPath, parentIndex, &dummyFileIndex, &toFileExist);
	if (toFileExist || isFolder(toPath, parentIndex)) {
		printString("Sudah ada file/folder dengan nama tersebut pada destinasi\r\n");
		return;
	}

	if (isFolder(fromPath, parentIndex)) {
		getFileNameFromPath(fromPath, fileName);
		printString(fileName);
		printString(" adalah sebuah folder\r\n");
		return;
	}

	searchFile(fromPath, parentIndex, &fileIndex, &fromFileExist);
	if (fromFileExist) {
		getDirIdxFromPath(toPath, parentIndex, &dirIndex, &dirValid);
		if (dirValid) {
			emptyFilesIndex = -1;
			for(i = 0; i < 64; i++) {
				if (files[i * 16] == 0x0 && files[i * 16 + 1] == 0x0 && files[i * 16 + 2] == 0x0) {
					emptyFilesIndex = i;
					break;
				}
			}

			if (emptyFilesIndex == -1) {
				printString("Tidak cukup entri di files\r\n");
				return;
			}

			getFileNameFromPath(toPath, fileName);
			if (fileName[0] == 0x0) {
				printString("Tidak ada nama file destinasi\r\n");
			}

			clear(files + emptyFilesIndex * 16, 16);
			files[emptyFilesIndex * 16] = dirIndex;
			files[emptyFilesIndex * 16 + 1] = files[fileIndex * 16 + 1];
			for(i = 0; i < 14; i++) {
				files[emptyFilesIndex * 16 + 2 + i] = fileName[i];
			}
		} 
		else {
			printString("Folder tidak valid\r\n");
			return;
		}
	}
	else {
		printString("File tidak ditemukan\r\n");
		return;
	}

	writeSector(files, 0x101);
	writeSector(files + 512, 0x102);
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

void executeProgram(char *filename, int segment, int *success, char parentIndex) {
    // Buat buffer
    int isSuccess;
    char fileBuffer[512 * 16];
    // Buka file dengan readFile
    readFile(&fileBuffer, filename, &isSuccess, parentIndex);
    // If success, salin dengan putInMemory
    if (isSuccess) {
        // launchProgram
        int i = 0;
        for (i = 0; i < 512*16; i++) {
            putInMemory(segment, i, fileBuffer[i]);
        }
        launchProgram(segment);
    } else {
        interrupt(0x21, 0, "File not found!", 0,0);
    }
}