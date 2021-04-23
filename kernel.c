/* Ini deklarasi fungsi */

void handleInterrupt21 (int AX, int BX, int CX, int DX);



void executeProgram(char *filename, int segment, int *success, char parentIndex);



void printShell(char parentIndex);



int currentDir;

char files[1024];



int main() {

	int success;

	executeProgram("printlogo", 0x4000, success, 0x00);

	//executeProgram("shell", 0x2000, success, 0x00);

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

void executeProgram(char *filename, int segment, int *success, char parentIndex) {

    // Buat buffer

    int isSuccess;

	int i;

    char fileBuffer[512 * 16];

    // Buka file dengan readFile

    readFile(&fileBuffer, filename, &isSuccess, parentIndex);

    // If success, salin dengan putInMemory

    if (isSuccess) {

        i = 0;

        for (i = 0; i < 512*16; i++) {

            putInMemory(segment, i, fileBuffer[i]);

        }

        launchProgram(segment);

    } else {

        interrupt(0x21, 0, "File not found!", 0,0);

    }

}