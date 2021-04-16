int main()
{
    char buffer[512];
    char arg[512];
    int argIdx;
    int i;

    readSector(buffer, 0x0F);

    argIdx = 0;
    if (buffer[4] != 0x0) {
		for (i = 4; buffer[i] != 0x0; i++) {
			arg[argIdx] = buffer[i];
			argIdx++;
			}
			cat(arg, currentDir);
		}
}