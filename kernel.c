/* Ini deklarasi fungsi */
void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void printLogo2(int baris, int kolom, char* s);
void printLogo();
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);

/* Utils */
int mod(int a, int b);
int div(int a, int b);
int strCompare(char *a, char *b, int length);
void getParentFromPath(char *path, char *fileSector, char parentIndex, char *dirIndex, char *fileName, int *result);

int main() {
	char files[1024];
	char fileName[14];
	char path[20];
	char dirIndex;
	int result;

	readSector(files, 0x101);
	readSector(files + 512, 0x102);

	readString(&path);
	getParentFromPath(path, files, 0x00, &dirIndex, fileName, &result);

	//test
	if (dirIndex == 0x03) {
		printString(&fileName);
	}
	else {
		printString("noob");
	}

	while(1){}
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
	default: 
		printString("Invalid interrupt"); 
	} 
}


void printString(char *string) {
	int i = 0;
  	while (printInterupt(&string[i])) {
    	interrupt(0x10, 0xe * 256 + string[i], 0, 0, 0);
    	i++;
  }
}

int printInterupt(char *string){
    return (*string != '\0' ? 1 : 0);
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

void readString(char* string)
{
	char input;
	char backspace = 0x8;
	char newl = 0xa;
	char enter = 0xd;
	char carriage = 0xd;
	char null = 0x0;
	int i = 0;
   	while(1){
		input = interrupt(0x16, 0, 0, 0, 0);
		if(input == backspace){
			if (i > 0) {
				interrupt(0x10, 0xe*256 + backspace, 0, 0, 0);
				interrupt(0x10, 0xe*256 + null, 0, 0, 0);		// Menghilangkan char di layar
				interrupt(0x10, 0xe*256 + backspace, 0, 0, 0);
				i--;
			}
		} else if(input == enter){		
			interrupt(0x10, 0xe*256 + newl, 0, 0, 0); 
			interrupt(0x10, 0xe*256 + carriage, 0, 0, 0); // Interrupt carriage return buat kembali ke line awal
			string[i] = newl;
			string[i+1] = carriage;
			string[i+2] = null;	// Akhir string
			return;
		} else {
			interrupt(0x10, 0xe*256 + input, 0, 0, 0); // Nampilin hurufnya ke layar 
			string[i] = input;
			i++;
		}
	}
}

void clear(char *buffer, int length) {
	int i;
	for(i = 0; i < length; i++) {
		buffer[i] = 0x0;
	}
}

void readSector(char *buffer, int sector) {
	interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector) {
	interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

int mod(int a, int b){
    while(a >= b){
        a = a - b;
    }
    return a;
}

int div(int a, int b) {
	int result = 0;
	while (a >= b) {
		a = a - b;
		result++;
	}
	return result;
}

int strCompare(char *a, char *b, int length) {
	int i;
	for (i = 0; i < length; i++) {
		if (a[i] != b[i]) {
			return 0;
		}
		if (a[i] == 0 || b[i] == 0) {
			return 1;
		}
	}
	return 1;
}

void getParentFromPath(char *path, char *fileSector, char parentIndex, char *dirIndex, char *fileName, int *result) {
	char dirName[14];
	char pathfileName[14];
	int fileIndex;
	int found;
	int i;
	int j;
	int k;

	clear(dirName, 14);
	clear(pathfileName, 14);

	*result = 1;
	*dirIndex = parentIndex;
	j = 0;
	for (i = 0; path[i] != 0x0; i++) {
    	if (path[i] != '/'){
    		pathfileName[j] = path[i];
			j++;
    	} else {
    		for (; j < 14; j++) {
        		pathfileName[j] = 0x0;
      		}

      		for (k = 0; k < 14; k++) {
        		dirName[k] = pathfileName[k];
      		}
			
			found = 0;
			for (fileIndex = 0; fileIndex < 64; fileIndex++) {
				if (fileSector[fileIndex * 16] == *dirIndex && fileSector[fileIndex * 16 + 1] == 0xFF && strCompare(dirName, fileSector + fileIndex * 16 + 2, 14)) {
					*dirIndex = fileIndex;
					found = 1;
					break;
				}
			}
			
			if (found) {
				j = 0;
			}
			else {
				*result = 0;
				break;
			}	
    	}
  	}

  	for (;j<14;j++) {
    	pathfileName[j] = 0x0;
  	}

	for (i = 0; i < 14; i++) {
        fileName[i] = pathfileName[i];
    }
}