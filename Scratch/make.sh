nasm naskfunc.asm -f elf32 -o naskfunc.o
gcc -ffreestanding -c kernel.c -m32 -o kernel.o
nasm kernel_entry.asm -f elf32 -o kernel_entry.o
nasm boot_sect_pm.asm -o boot_sect.bin
ld -o kernel.bin -m elf_i386 -Ttext 0x1000 kernel_entry.o naskfunc.o  kernel.o --oformat binary

cat boot_sect.bin kernel.bin > os.img
