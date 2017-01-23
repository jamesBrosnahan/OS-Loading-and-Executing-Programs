#Builds bootable disk image:

#Assembles bootload.asm
nasm bootload.asm

#Creates blank disk image.
dd if=/dev/zero of=floppya.img bs=512 count=2880

#Puts bootload into sector zero.
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc

#Places disk map in sector 1 of disk
dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc

#Places disk directory in sector 2 of disk.
dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc

#Compiles kernel.c
bcc -ansi -c -o kernel.o kernel.c

#Assembles kernel.asm
as86 kernel.asm -o kernel_asm.o

#Links kernel.o with kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o

#Puts kernel into sector three.
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3

#Places msg in sector 30 of disk
dd if=msg of=floppya.img bs=512 count=1 seek=30 conv=notrunc
