#include <stdio.h>

void main(int argc, char* argv[]) {
  	FILE* file_to_load;
	FILE* system;
	  
	char map[512];
	char files[1024];
	char sectors[512];

	char c;

	int i; 
	int fileIndex;
	int emptyFileIndex;
	int sectorIndex;
	int emptySectorIndex;
	int mapIndex;
	int emptyMapIndex;

	if (argc < 2) {
		printf("Ketik nama file yang ingin di-load\n");
		return;
	}

	file_to_load = fopen(argv[1], "r");
	if (file_to_load == 0) {
		printf("File tidak ditemukan\n");
		return;
	}
	system = fopen("system.img", "r+");
	if (system == 0) {
		printf("system.img tidak ditemukan\n");
		return;
	}

	fseek(system, 512 * 0x100, SEEK_SET);
	for (i = 0; i < 512; i++) { 
		map[i] = fgetc(system);
	}
	fseek(system, 512 * 0x101, SEEK_SET);
	for (i = 0; i < 1024; i++) {
		files[i] = fgetc(system);
	}
	fseek(system, 512 * 0x103, SEEK_SET);
	for (i = 0; i < 512; i++) {
		sectors[i] = fgetc(system);
	}

	emptyFileIndex = -1;
	for (fileIndex = 0; fileIndex < 64; fileIndex++) {
		if (files[fileIndex * 16] == 0 && files[fileIndex * 16 + 1] == 0 && files[fileIndex * 16 + 2] == 0) {
			emptyFileIndex = fileIndex;
			break;
		} 
	}
	if (emptyFileIndex == -1) {
		printf("Tidak cukup entri di files\n");
		return;
	}

	emptySectorIndex = -1;
	for (sectorIndex = 0; sectorIndex < 32; sectorIndex++)
		if (sectors[sectorIndex * 16] == 0) {
			emptySectorIndex = sectorIndex;
			break;
		}
	if (emptySectorIndex == -1) {
		printf("Tidak cukup sektor kosong\n");
		return;
	}
	
	// clear
	for (i = 0; i < 14; i++) {
		files[emptyFileIndex * 16 + 2 + i] = 0x0;
	}
	files[emptyFileIndex * 16] = 0xFF;
	files[emptyFileIndex * 16 + 1] = emptySectorIndex;
	for (i = 0; i < 14; i++) {
		if (argv[1][i] == 0) {
			break;
		}
		files[emptyFileIndex * 16 + 2 + i] = argv[1][i];
	}

	int sectcount = 0;
	while (!feof(file_to_load)) {
		if (sectcount == 16) {
			printf("Ukuran file terlalu besar\n");
			return;
		}

		emptyMapIndex = -1;
		for (mapIndex = 0; mapIndex < 512; mapIndex++) {
			if (map[mapIndex] == 0) {
				emptyMapIndex = mapIndex;
				break;
			}
		}
		if (emptyMapIndex == -1) {
			printf("Tidak cukup sektor kosong\n");
			return;
		}

		map[emptyMapIndex] = 0xFF;
		sectors[emptySectorIndex] = emptyMapIndex;
		
		printf("File %s diletakan pada sektor %d\n", argv[1], emptyMapIndex);

		fseek(system, emptyMapIndex * 512, SEEK_SET);
		for (i = 0; i < 512; i++) {
			if (feof(file_to_load)) {
				fputc(0x0, system);
				break;
			} else {
				c = fgetc(file_to_load);
				fputc(c, system);
			}
		}

		emptySectorIndex++;
		sectcount++;
	}

	fseek(system, 512 * 0x100, SEEK_SET);
	for (i = 0; i < 512; i++) {
		fputc(map[i], system);
	}
	fseek(system, 512 * 0x101, SEEK_SET);
	for (i = 0; i < 1024; i++) {
		fputc(files[i], system);
	}
	fseek(system, 512 * 0x103, SEEK_SET);
	for (i = 0; i < 512; i++){
		fputc(sectors[i], system);
	}

	printf("Load file berhasil");

	fclose(system);
	fclose(file_to_load);

	return;
}