void printLogo2(int baris, int kolom, char* s);
void printLogo();

void main() {
    int success;
    printLogo();
    executeProgram("shell", 0x2000, &success, 0x00);
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