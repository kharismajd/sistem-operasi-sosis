#include "fileio.h"
#include "folderio.h"
#include "text.h"

int countFreeSector();
int countFreeFilesEntry();
int countFreeSectorExtry();

int main()
{
    char fileBuffer[8192];
    char files[1024];
    char map[512];
    char sectors[512];
    char buffer[512];
    char path[64];
    char path2[64];
    char fileName[14];
    char argv[4][64];
    char currDir;
    char fileIdx;
    char sectorIdx;
    char mapIdx;
    char isFileExist;
    
    int dummyResult;
    int argc;
    int neededSector;
    int countFiles;
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
            i += 1;
        }
        
        argv[argc][nameLength] = buffer[i];
        nameLength += 1;
        i += 1;
    }
    argc += 1;

    if (argc < 2)
    {
        printString("Cara menggunakan: cp <file> <file baru>\r\n");
        printString("Cara menggunakan: cp <file> <folder destinasi>\r\n");
        printString("Cara menggunakan: cp <file 1> <file 2> <file n> <folder destinasi>\r\n");
        return;
    }

    //Hitung sector yang dibutuhkan
    countFiles = argc - 1;
    neededSector = 0;
    for (i = 0; i < countFiles; i++)
    {
        clear(path, 64);
        strcpy(argv[i], path, 64);
        if (!isFolder(path, currDir))
        {
            searchFile(path, currDir, &fileIdx, &isFileExist);
            if (!isFileExist)
            {
                printString("Tidak bisa menemukan file ");
                printString(path);
                printString("\r\n");
                return;
            }

            sectorIdx = files[fileIdx * 16 + 1];
            for (i = 0; i < 16; i++)
            {
                if (sectors[sectorIdx * 16 + i] != 0x0)
                {
                    neededSector += 1;
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            printString(path);
            printString(" adalah sebuah folder\r\n");
            return;
        }
    }

    //Cek jika cp memungkinkan
    if (neededSector > countFreeSector())
    {
        printString("Tidak cukup sektor\r\n");
        return;
    }

    if (countFiles > countFreeFilesEntry())
    {
        printString("Tidak cukup entri files\r\n");
        return;
    }

    if (countFiles > countFreeSectorExtry())
    {
        printString("Tidak cukup entri sektor\r\n");
        return;
    }

    clear(path, 64);
    strcpy(argv[argc - 1], path, 64);
    if (isFolder(path, currDir))
    {
        for (i = 0; i < countFiles; i++)
        {
            clear(fileBuffer, 8192);
            clear(path, 64);
            clear(path2, 64);

            strcpy(argv[argc - 1], path2, 64)
            strcpy(argv[i], path, 64);
            getFileNameFromPath(path, fileName);

            concat(path2, "/");
            concat(path2, fileName);

            readFile(fileBuffer, path, &dummyResult, currDir);
            writeFile(fileBuffer, path2, &dummyResult, currDir);
        }
    }
    else
    {
        if (argc == 2)
        {
            clear(fileBuffer, 8192);

            clear(path, 64);
            strcpy(argv[0], path, 64);
            readFile(fileBuffer, path, &dummyResult, currDir);

            clear(path, 64);
            strcpy(argv[1], path, 64);
            writeFile(fileBuffer, path, &dummyResult, currDir);
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

int countFreeFilesEntry()
{
    char files[1024];
    int i, count;

    readSector(files, 0x101);
    readSector(files + 512, 0x102);

    count = 0;
    for (i = 0; i < 64; i++)
    {
        if (files[16 * i] == 0x0 && files[16 * i + 1] == 0x0 && files[16 * i + 2] == 0x0)
        {
            count += 1;
        }
    }

    return count;
}

int countFreeSectorExtry()
{
    char sectors[512];
    int i, count;

    readSector(sectors, 0x103);

    count = 0;
    for (i = 0; i < 32; i++)
    {
        if (sectors[16 * i] == 0x0)
        {
            count += 1;
        }
    }

    return count;
}