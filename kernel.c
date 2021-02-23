void printString(char *charac) {

  while (printInterupt(*charac)) {
    interrupt(0x10, 0xe * 256 + *charac, 0, 0, 0);
    charac++;
  }
}

int printInterupt(char *charac){
    return (*charac != '\0' ? 1 : 0);
}
