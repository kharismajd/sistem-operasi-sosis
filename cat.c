#include "fileio.h"
#include "folderio.h"
 
int main()
{
    char buffer[512];
    char arg[512];
 
    char fileBuffer[8192];
    char fileName[14];
 
    char currDir;
 
    int result;
    int argIdx;
    int i;
 
    clear(buffer, 512);
    clear(arg, 512);
    clear(fileBuffer, 8192);
    clear(fileName, 14);
    readSector(buffer, 511);
 
    currDir = buffer[0];
    argIdx = 0;
    if (buffer[5] != 0x0) {
		for (i = 5; buffer[i] != 0x0; i++) {
			arg[argIdx] = buffer[i];
			argIdx++;
		}
 
        if (isFolder(arg, currDir)) {
            getFileNameFromPath(arg, fileName);
            printString(fileName);
            printString(" adalah sebuah folder\r\n");
            executeProgram("shell", 0x4000, &result, 0x00);
        }

        readFile(fileBuffer, arg, &result, currDir);
 
        if (result == 1) {
            printString(fileBuffer);
            printString("\r\n");
        }
	}
    executeProgram("shell", 0x4000, &result, 0x00);
}