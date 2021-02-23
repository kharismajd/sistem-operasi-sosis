int main () {
	putInMemory(0xB000,0x8000,'H');
	putInMemory(0xB000,0x8001,0xD);
	putInMemory(0xB000,0x8002,'a');
	putInMemory(0xB000,0x8003,0xD);
	putInMemory(0xB000,0x8004,'i');
	putInMemory(0xB000,0x8005,0xD);
	while (1);
}

void handleInterrupt21 (int AX, int BX, int CX, int DX) {}

void printString(char *charac) {

  while (printInterupt(*charac)) {
    interrupt(0x10, 0xe * 256 + *charac, 0, 0, 0);
    charac++;
  }
}

int printInterupt(char *charac){
    return (*charac != '\0' ? 1 : 0);
}
