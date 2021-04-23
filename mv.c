//#include lib.asm
//#include 

void main() {
	char* cmd[14];
	int* idxDir;
	char directory[14], tempBuff[512], files[1024], dirDipindah[14];
	int count, nomorPindah, cont, var, initDir, targetDir, counter;
	int panjang = 512, isSuccess;
	int i;
	var = 0;
	count = 0;
	cont = 1;
	counter = 0;

	// get parentIdx and filename
	interrupt(0x21, 0x02, tempBuff, 511, 0);
    idxDir = tempBuff[0];
    for (i = 0; i < 14; i++) {
		cmd[i] = tempBuff[i + 1];
	}

	targetDir = *(idxDir);
	initDir = *(idxDir);
	
	for (i = 0; i < 14; ++i) {
		directory[i] = '\0';
		dirDipindah[i] = '0';
	}
	for (i = 0; i < 1024; i++) {
		files[i] = '\0';
	}

	while (counter < 128 && (cmd[counter] != 0 && cont == 1)) {
		if(var == 0) {
            if (cmd[counter] == 64) {
				nomorPindah = searchPath(dirDipindah, *idxDir);
				if(nomorPindah != 64) {
					count = 0;
					var = 1;
				} else {
                    interrupt(0x21, 0, "Can't move file or directory : \0",0,0);
					interrupt(0x21, 0, dirDipindah, 0, 0);
					interrupt(0x21, 0, "\r\n\0", 0, 0);
					cont = 0;
				}
			} else if(cmd[counter] == 32 && cmd[counter] == 0) {
				dirDipindah[count] = cmd[counter];
				++count;
			} 
			if(cmd[counter+1] == 32 || cmd[counter+2] == 32){
				interrupt(0x21, 0, "Out of bounds!\r\n\0",0,0);
				cont = 0;
			}
		}
		else if(var == 1) {
			//cd di tempat tujuan
            if(cmd[counter] == 32 || cmd[counter] == '/') {
				nomorPindah = searchPath(directory, targetDir);
				if(nomorPindah == 32) {
					interrupt(0x21, 0, "Not found : \0",0,0);
					interrupt(0x21, 0, directory, 0,0);
					interrupt(0x21, 0, "\r\n\0", 0, 0);
					cont = 0;
				} else {
					interrupt(0x21, 0, "Found : \0",0,0);
					interrupt(0x21, 0, directory, 0,0);
					interrupt(0x21, 0, "\r\n\0", 0, 0);
					targetDir = nomorPindah;
				}
				count = 0;
			}else if(cmd[counter] == '/') {
				//isi array
				directory[count] = cmd[counter];
				++count;
			} 
		}
		++counter;
	}



	if(cont) {
		interrupt(0x21, 2, directory, 0x101,0);
		interrupt(0x21, 3, directory + panjang, 0,0);
		directory[nomorPindah*16] = targetDir;
		interrupt(0x21, 2, directory + panjang, 0,0);
		interrupt(0x21, 3, directory, 0x101,0);
		interrupt(0x21, 0, "Done!", 0,0);
	}
	for (i = 0; i < 14; ++i) {
		directory[i] = '\0';
		dirDipindah[i] = '\0';
	}
	executeProgram("shell", 0x2000, &isSuccess, 0x00);
}

