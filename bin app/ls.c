int main()
{
    char buffer[512];
    char files[1024];
	char fileName[14];

    char currDir;

	int fileIndex;
    int i;

    readSector(buffer, 0x0F);
	readSector(files, 0x101);
	readSector(files + 512, 0x102);

    currDir = buffer[0];
    if (buffer[3] == 0x0 || buffer[3] == " ")
    {
        for(fileIndex = 0; fileIndex < 64; fileIndex++)
        {
            if (files[fileIndex * 16] == currDir && files[fileIndex * 16 + 2] != 0x0) {
                clear(fileName, 14);
                for(i = 0; i < 14; i++) {
                    fileName[i] = files[fileIndex * 16 + 2 + i];
                }
                printString(fileName);
                printString("\r\n");
            }
        }
	}	
}