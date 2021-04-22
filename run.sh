# Persiapan disk image
dd if=/dev/zero of=system.img bs=512 count=2880 
# Bootloader
nasm bootloader.asm -o bootloader
dd if=bootloader of=system.img bs=512 count=1 conv=notrunc
 
# Setup img
dd if=map.img of=system.img bs=512 count=1 seek=256 conv=notrunc
dd if=files.img of=system.img bs=512 count=2 seek=257 conv=notrunc
dd if=sectors.img of=system.img bs=512 count=1 seek=259 conv=notrunc
 
# Setup library & kernel
bcc -ansi -c -o kernel.o kernel.c
bcc -ansi -c -o math.o math.c
bcc -ansi -c -o text.o text.c
bcc -ansi -c -o fileio.o fileio.c
nasm -f as86 lib.asm -o lib_asm.o
bcc -ansi -c -o folderio.o folderio.c
nasm -f as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o math.o text.o fileio.o folderio.o
dd if=kernel of=system.img bs=512 conv=notrunc seek=1
 
# Creating loadFile
gcc -o loadFile loadFile.c
./loadFile test/test1.txt

# compile shell
bcc -ansi -c -o shell.o shell.c
nasm -f as86 lib.asm -o lib_asm.o
ld86 -o shell -d shell.o lib_asm.o
./loadFile shell
 
# compile cat
bcc -ansi -c cat.c -o cat.o
ld86 -o cat -d cat.o lib_asm.o text.o fileio.o math.o
./loadFile cat
 
# compile rm
bcc -ansi -c rm.c -o rm.o
ld86 -o rm -d rm.o lib_asm.o text.o fileio.o folderio.o math.o
./loadFile rm
 
# compile mkdir
bcc -ansi -c mkdir.c -o mkdir.o
ld86 -o mkdir -d mkdir.o lib_asm.o
./loadFile mkdir

# loadFile testcases
./loadFile test/test1.txt
./loadFile test/test2.txt
./loadFile test/test3.txt
 
# Last
echo c | bochs -f if2230.config
 