/* Ini deklarasi fungsi */
void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void printLogo();
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0

int main() {
	char x[100];
	printLogo();
	readString(&x);
	printString(&x);
	readString(&x);
	printString(&x);
	while(1);
}

void handleInterrupt21 (int AX, int BX, int CX, int DX){
    switch (AX) {
    case 0x0:
        printString(BX);
        break;
    case 0x1:
        readString(BX);
        break;
    default:
        printString("Invalid interrupt");
    }
}

void printString(char *string) {
	char enter = 0xd;
	int i = 0;
  	while (printInterupt(&string[i])) {
    	interrupt(0x10, 0xe * 256 + string[i], 0, 0, 0);
    	i++;
  }
}

int printInterupt(char *string){
    return (*string != '\0' ? 1 : 0);
}

void printLogo() {
	printString("----------\n");
	printString("sOSissOSis\n");
	printString("OSissOSiss\n");
	printString("sissOSisso\n");
	printString("issOSissOS\n");
	printString("ssOSissOSi\n");
	printString("----------\n");
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