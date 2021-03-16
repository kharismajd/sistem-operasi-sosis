void execProg(char* progName, char parentIndex);
int cd(char* cmd, int idxDir);
unsigned char compareStr(char* strA, char* strB);
int compareStrN(char* strA, char* strB, int n);
int searchPath(char* dirCall, int parentIndex);

int currentDir, dirBefore, dirGanti, itrDirName;
char currentDirName[128], directoryBuffer[1024];
currentDir = 0xFF;

int main() {
	char arg[14];
	char* input;
	int suc, i;
	currentDir = 0xFF;
	itrDirName = 0;
	dirGanti = 0;
	dirBefore = 0;

	i = 0;

	while (1) {
		do {
			interrupt(0x21, 0x2, directoryBuffer, 0x101, 0);
			interrupt(0x21, 0x2, directoryBuffer + 512, 0x102, 0);
			interrupt(0x21, 0x00, "\r\nRoot", 0, 0);
			if (!(dirBefore == 0)) {
				while (!(currentDirName[itrDirName] == '/')) {
					currentDirName[itrDirName--] = '\0';
				}
				currentDirName[itrDirName] = '\0';
				dirBefore = 0;
				
			}
			interrupt(0x21, 0x00, currentDirName, 0, 0);
			interrupt(0x21, 0x00, ">", 0, 0);
			interrupt(0x21, 0x01, input, 1, 0);
		} while (compareStr(input, ""));
		interrupt(0x21, 0x00, "\r\n", 0, 0);
		
		if (compareStrN(input, "./", 2)) {
			// interrupt(0x21, 0x00, "masuk if exec\r\n", 0, 0);
			for (i = 0; i < 14; i++) {
				arg[i] = 0x0;
			}
			i = 2;
			while (i < 16 ) {
				if (input[i] == '\0') {
					break;
				} else {
					arg[i - 2] = input[i];
				}
				i++;
			}

			interrupt(0x21, 0x00, arg, 0, 0);
			interrupt(0x21, 0x00, "\r\n", 0, 0);

			execProg(arg, 0xFF);
		} else if(compareStrN(input,"cd", 2)) {
			i = 3;
			while (i < 17 ) {
				if (input[i] == '\0') {
					break;
				} else {
					arg[i - 3] = input[i];
				}
				i++;
			}

			while (i < 17) {
				arg[i-3] = '\0';
				i++;
			}

			currentDir = cd(arg, currentDir);
		} else {
			interrupt(0x21, 0x00, "Perintah Invalid\r\n", 0, 0);
		}
	}
	return 0;
}

int cd(char* cmd, int idxDir) {
	char directory[14];
	int i, cnt, val, cont, initDir, k;
	cnt = 0;
	cont = 1;
	i=0;
	initDir = idxDir;
	for(i;i<14;++i) {
		directory[i] = '\0';
	}
	i = 0;
	while(i<128 && cmd[i] != '\0' && cont == 1) {
		if(cmd[i] != '/') {
			directory[cnt] = cmd[i];
			cnt++;
		} else if(cmd[i] == '/') {
			val = searchPath(directory, initDir);
			if(val == 0x100) {
				cont = 0;
			} else {
				initDir = val;
				currentDirName[itrDirName++] = '/';
				k = 0;
				while (k < 14 ) {
					if (directoryBuffer[initDir * 16 + 2 + k] == '\0') {
						break;
					} else {
						currentDirName[itrDirName + k] = directoryBuffer[k + initDir * 16 + 2];
						k++;
					}
				}
				itrDirName += k;
			}
			cnt = 0;
		}
		++i;
	}
	if(cont) {
		val = searchPath(directory, initDir);
		if(val == 0x100) {
			cont = 0;
		} else {
			//interrupt(0x21, 0, "Masuk sini bro2!\r\n",0,0);
			if (dirBefore ==0) {
				initDir = val;
				currentDirName[itrDirName++] = '/';
				k = 0;
				while (k < 14 ) {
					if (directoryBuffer[initDir * 16 + 2 + k] == '\0') {
						break;
					} else {
						currentDirName[itrDirName + k] = directoryBuffer[k + initDir * 16 + 2];
						k++;
					}
				}
				itrDirName += k;
			}
			initDir = val;
			dirGanti = 1;
		}
		cnt = 0;
	}

	return initDir;
}

int searchPath(char* dirCall, int parentIndex) {
	char directoryBuffer[1024], filename[14];
	int i, found, var, h, isNameMatch, k;
	i=0;
	interrupt(0x21, 2, directoryBuffer, 0x101, 0);
	interrupt(0x21, 2, directoryBuffer+512, 0x102, 0);
	found = 0;
	if(dirCall[0] == '.' && dirCall[1] == '.') {
		if(parentIndex != 0xFF) {
			var = directoryBuffer[parentIndex*16];
			dirBefore = 1;
		} else {
			var = 0xFF;
		}
		found = 1;
	} else {
		i = 0;
		while (found == 0 && i < 64) {
			if(directoryBuffer[i*16] == parentIndex && directoryBuffer[16 * i+1] == 0xFF){
				h = 0;
				isNameMatch = 1;
				for (h = 0; h < 14; h++) {
					if (dirCall[h] != directoryBuffer[h + i * 16 + 2]) {
						isNameMatch = 0;
						break;
					} else if (directoryBuffer[h + i * 16 + 2] == '\0' && dirCall[h] == '\0') {
						break;
					}
				}

				if (isNameMatch) {
					var = i;
					found = 1;
					k = 0;
					while (k < 14 ) {
						if (directoryBuffer[i * 16 + 2 + k] == '\0') {
							break;
						} else {
							filename[k] = directoryBuffer[i * 16 + 2 + k];
						}
						k++;
					}

					while (k < 16) {
						filename[k] = '\0';
						k++;
					}
				}
			}
			++i;
 		}	
	}
	if(found) {
		return var;
	} else {
		return 0x100;
	}
}

void execProg(char* progName, char parentIndex) {
	int isFound = 0, k, h, isNameMatch, suc, i;
	char files[1024];

	interrupt(0x21, 0, "masuk execProg:(\r\n", 0, 0);

	for (i = 0; i < 1024; i++ ) {
		files[i] = 0x0;
	}
	interrupt(0x21, 0x2, files, 0x101, 0);
	interrupt(0x21, 0x2, files + 512, 0x102, 0);

	interrupt(0x21, 0x00, progName, 0, 0);
	interrupt(0x21, 0x00, "\r\n", 0, 0);

	k = 0;
	while (k < 64 && !isFound) {
		if (files[k * 16] == 0xFF && files[k * 16 + 1] != 0xFF) {
			h = 0;
			isNameMatch = 1;
            for (h = 0; h < 14; h++) {
                if (progName[h] != files[h + k * 16 + 2]) {
	                isNameMatch = 0;
					break;
        	    } else if (files[h + k * 16 + 2] == '\0' && progName[h] == '\0') {
            		break;
            	}
        	}

        	if (isNameMatch) {
        		isFound = 1;
        	}
		}
		k++;
	}

	if (isFound) {
		interrupt(0x21, 0, "Executing Program...\r\n", 0, 0);
		interrupt(0x21, 0xFF06, progName, 0x2000, &suc);
	} else {
		interrupt(0x21, 0, "Program tidak ditemukan:(\r\n", 0, 0);
	}
}

unsigned char compareStr(char* strA, char* strB) {
	int i = 0;
	unsigned char sama = 1;
	while (i < 128 && sama) {
		if(strA[i] != strB[i]) {
			sama = 0;
		} else if(strA[i] == 0x0 && strB[i] == 0x0) {
			i = 128;
		}
		++i;
	}
	return sama;
}

int compareStrN(char* strA, char* strB, int n) {
	int i = 0;
	while (!(strA[i] == '\0' && strB[i] == '\0') && i < n) {
		if (strA[i] != strB[i]) {
            return 0;
        }
		++i;
	}
	return 1;
}