# Enable kebebasan pada run.sh
chmod +x run.sh

# Persiapan disk image
dd if=/dev/zero of=system.img bs=512 count=2880

# Bootloader
nasm bootloader.asm -o bootloader

# Bootloader dimasukkan ke system.img
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
bcc -ansi -c -o folderio.o folderio.c
nasm -f as86 lib.asm -o lib_asm.o
nasm -f as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o text.o fileio.o math.o folderio.o
dd if=kernel of=system.img bs=512 conv=notrunc seek=1
 
# Creating loadFile
gcc -o loadFile loadFile.c
./loadFile test1.txt

# Compile hello
bcc -ansi -c hello.c -o hello.o
ld86 -o hello -d hello.o text.o kernel.o kernel_asm.o fileio.o folderio.o math.o
./loadFile hello

# Compile shell
bcc -ansi -c shell.c -o shell.o
ld86 -o shell -d shell.o fileio.o folderio.o text.o math.o kernel_asm.o kernel.o
./loadFile shell

# Compile cat
bcc -ansi -c cat.c -o cat.o
ld86 -o cat -d cat.o fileio.o folderio.o text.o math.o kernel_asm.o kernel.o
./loadFile cat

# Compile cp
bcc -ansi -c cp.c -o cp.o
ld86 -o cp -d cp.o fileio.o folderio.o text.o math.o kernel_asm.o kernel.o
./loadFile cp

# Compile ln
bcc -ansi -c ln.c -o ln.o
ld86 -o ln -d ln.o fileio.o text.o folderio.o  math.o kernel.o kernel_asm.o
./loadFile ln

# Compile mv
bcc -ansi -c mv.c -o mv.o
ld86 -o mv -d mv.o lib_asm.o folderio.o fileio.o math.o text.o kernel.o kernel_asm.o
./loadFile mv

# Compile rm
bcc -ansi -c rm.c -o rm.o
ld86 -o rm -d rm.o text.o folderio.o fileio.o lib_asm.o math.o kernel.o kernel_asm.o
./loadFile rm

# Compile mkdir
bcc -ansi -c mkdir.c mkdir.o
ld86 -o mkdir -d mkdir.o text.o folderio.o fileio.o lib_asm.o math.o kernel.o kernel_asm.o
./loadFile mkdir

# Delete unnecessary files
rm *.o bochsout.txt bootloader cat kernel hello
rm loadFile mkdir rm shell ln cp mv
 
# Last
echo c | bochs -f if2230.config
 