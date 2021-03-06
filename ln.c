int main()
{
    char files[1024];
    char buffer[512];
    char arg1[64];
    char arg2[64];
    char fileName[14];

    char fileIndex;
    char dummyFileIndex;
    char emptyFilesIndex;
    char dirIndex;
    char fileSector;
    char currDir;

    int fromFileExist;
    int toFileExist;
    int dirValid;
    int arg1Idx;
    int arg2Idx;
    int badCommand;
    int i;
    int success;

    clear(files[1024]);
    clear(buffer[512]);
    clear(arg1[64]);
    clear(arg2[64]);
    clear(fileName[14]);
    readSector(buffer, 511);

    currDir = buffer[0];
    for (i = 4; buffer[i] != ' '; i++)
    {
        if (buffer[i] == 0x0)
        {
            badCommand = 1;
            break;
        }
        arg1[arg1Idx] = buffer[i];
        arg1Idx++;
    }

    i++;
    for (; buffer[i] != 0x0; i++)
    {
        arg2[arg2Idx] = buffer[i];
        arg2Idx++;
    }

    if (!badCommand)
    {
        readSector(files, 0x101);
        readSector(files + 512, 0x102);

        searchFile(arg2, currDir, &dummyFileIndex, &toFileExist);
        if (toFileExist || isFolder(arg2, currDir))
        {
            printString("Sudah ada file/folder dengan nama tersebut pada destinasi\r\n");
            executeProgram("shell", 0x2000, &success, 0x00);
        }

        if (isFolder(arg1, currDir))
        {
            getFileNameFromPath(arg1, fileName);
            printString(fileName);
            printString(" adalah sebuah folder\r\n");
            executeProgram("shell", 0x2000, &success, 0x00);
        }

        searchFile(arg1, currDir, &fileIndex, &fromFileExist);
        if (fromFileExist)
        {
            getDirIdxFromPath(arg2, currDir, &dirIndex, &dirValid);
            if (dirValid)
            {
                emptyFilesIndex = -1;
                for (i = 0; i < 64; i++)
                {
                    if (files[i * 16] == 0x0 && files[i * 16 + 1] == 0x0 && files[i * 16 + 2] == 0x0)
                    {
                        emptyFilesIndex = i;
                        break;
                    }
                }

                if (emptyFilesIndex == -1)
                {
                    printString("Tidak cukup entri di files\r\n");
                    executeProgram("shell", 0x2000, &success, 0x00);
                }

                getFileNameFromPath(arg2, fileName);
                if (fileName[0] == 0x0)
                {
                    printString("Tidak ada nama file destinasi\r\n");
                    executeProgram("shell", 0x2000, &success, 0x00);
                }

                clear(files + emptyFilesIndex * 16, 16);
                files[emptyFilesIndex * 16] = dirIndex;
                files[emptyFilesIndex * 16 + 1] = files[fileIndex * 16 + 1];
                for (i = 0; i < 14; i++)
                {
                    files[emptyFilesIndex * 16 + 2 + i] = fileName[i];
                }
            }
            else
            {
                printString("Folder tidak valid\r\n");
                executeProgram("shell", 0x2000, &success, 0x00);
            }
        }
        else
        {
            printString("File tidak ditemukan\r\n");
            executeProgram("shell", 0x2000, &success, 0x00);
        }

        writeSector(files, 0x101);
        writeSector(files + 512, 0x102);
    }
    else
    {
        printString("Tidak ada file operand\r\n");
        executeProgram("shell", 0x2000, &success, 0x00);
    }
    executeProgram("shell", 0x4000, &success, 0x00);
}