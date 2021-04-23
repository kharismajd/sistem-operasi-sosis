void main() {
    int success;
    printString("Hello, World!\r\n\n");
    executeProgram("shell", 0x4000, &success, 0x00);
}
