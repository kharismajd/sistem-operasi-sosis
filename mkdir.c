#include "fileio.h"
#include "folderio.h"

int main() {
    char buffer[512];
    char arg[14];
    char parentIndex;

    int argIdx;
    int i;
    int success;

    clear(buffer, 512);
    clear(arg, 14);
    readSector(buffer, 511);

    parentIndex = buffer[0];
    argIdx = 0;
    if (buffer[7] != 0x0) {
		for (i = 7; buffer[i] != 0x0; i++) {
			arg[argIdx] = buffer[i];
			argIdx++;
		}
        makeDir(arg, parentIndex);
    }
    executeProgram("shell", 0x2000, &success, 0x00);
}