/* Ini deklarasi fungsi */
void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printLogo2(int baris, int kolom, char* s);
void printLogo();
void executeProgram(char *filename, int segment, int *success, char parentIndex);
<<<<<<< HEAD
void cd(char *path, char *parentIndex);
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0
void printShell(char parentIndex);

int currentDir;
char files[1024];

int main() {
	int issukses;
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

	executeProgram("shell",0x2000,&issukses,0xFF);
	while(1) {
		readSector(files, 0x101);
		readSector(files + 512, 0x102);

		printShell(currentDir);
		
		clear(input, 64);
		clear(tempBuff, 512);
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
=======

int main() {
	int issukses;

	printLogo();
	printString("\n\n\n\n");

	executeProgram("shell",0x2000,&issukses,0xFF);
	while(1);

>>>>>>> 3599e434655dccb0e32c92cc675d793e353c62e4
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