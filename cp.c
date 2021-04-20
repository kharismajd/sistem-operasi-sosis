int main()
{
    char files[1024];
    char map[512];
    char sectors[512];
    char buffer[512];
    char fileName[14];
    char argv[8][14];
    char currDir;
    
    int argc;
    int i;
    int nameLength;

    readSector(buffer, 0x0F);
    readSector(map, 0x100);
	readSector(files, 0x101);
	readSector(files + 512, 0x102);
	readSector(sectors, 0x103);

    currDir = buffer[0];

    //Baca argumen
    i = 4;
    argc = 0;
    nameLength = 0;
    while (buffer[i] != 0x0)
    {
        if (buffer[i] == ' ')
        {
            nameLength = 0;
            argc += 1;
        }
        
        argv[argc][nameLength] = buffer[i];
        nameLength += 1;
    }
    argc += 1;

    if (argc < 2)
    {
        printString("Cara menggunakan: cp <file> <file baru>\r\n");
        printString("Cara menggunakan: cp <file 1> <file 2> <file n> <folder destinasi>\r\n");
        return;
    }

    strcpy(argv[argc - 1], fileName, 14);
    if (isFolder(fileName, currDir))
    {
        //Buat file baru di folder dengan nama file yang sama
    }
    else
    {
        if (argc == 2)
        {
           //Buat file baru di folder yang sama dengan nama file argv[1]
        }
        else
        {
            printString("Destinasi bukan folder");
            return;
        }
    }

}

int countFreeSector()
{
    char map[512];
    int i, count;

    readSector(map, 0x100);

    count = 0;
    for (i = 0; i < 512; i++)
    {
        if (map[i] == 0x0)
        {
            count += 1;
        }
    }
    return count;
}