

int isFolder(char *path, char parentIndex) {
	char files[1024];
	char fileName[14];
	char dirIdx;
	char fileIdx;

	int dirValid;

	readSector(files, 0x101);
	readSector(files + 512, 0x102);

	getDirIdxFromPath(path, parentIndex, &dirIdx, &dirValid);
	if (dirValid) {
		getFileNameFromPath(path, fileName);
		for (fileIdx = 0; fileIdx < 64; fileIdx++) {
			if (files[fileIdx * 16] == dirIdx && files[fileIdx * 16 + 1] == 0xFF && strCompare(fileName, files + fileIdx * 16 + 2, 14)) {
				return 1;
			}
		}
	}
	return 0;
}