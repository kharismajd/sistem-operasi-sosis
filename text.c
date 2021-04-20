#include "text.h"

void printString(char *string) {
	int i = 0;
  	while (printInterupt(&string[i])) {
    	interrupt(0x10, 0xe * 256 + string[i], 0, 0, 0);
    	i++;
  }
}

int printInterupt(char *string) {
    return (*string != '\0' ? 1 : 0);
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
			string[i] = null;
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

int strCompare(char *a, char *b, int length) {
	int i;
	for (i = 0; i < length; i++) {
		if (a[i] != b[i]) {
			return 0;
		}
		if (a[i] == 0x0) {
			return 1;
		}
	}
	return 1;
}