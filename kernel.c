/* Ini deklarasi fungsi */
void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void printLogo2(int baris, int kolom, int warna, int latar, char* s);
void printLogo(int color, int bg);
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0

int main() {
	char x[100];
	putInMemory(0xB000,0x8000,'H');
	printLogo(0x0F,0x03);
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

void printLogo2(int baris, int kolom, int warna, int latar, char* s){
   int i = 0;
   int offset=0x8000+((row-1)*80*2)+(40-ln/2)*2;
   while(s[i]!='\0'){
      putInMemory(0xB000, offset + i*2, s[i]);
      putInMemory(0xB000, offset + i*2+1, color + (bg<<4));
      i++;
   }
}
void printLogo(int color, int bg){
   printLogo2(7, 20, color, bg,"---------");
   printLogo2(8, 20, color, bg,"sOSissOSis");
   printLogo2(9, 20, color, bg,"sissOSisso");
   printLogo2(10, 20, color, bg,"issOSissOS");
   printLogo2(11, 20, color, bg,"ssOSissOSi");
   printLogo2(12, 20, color, bg,"---------");
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