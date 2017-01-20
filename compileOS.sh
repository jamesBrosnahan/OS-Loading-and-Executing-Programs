#Builds bootable disk image:

#Creates blank disk image.
dd if=/dev/zero of=floppya.img bs=512 count=2880

#Assembles bootload.asm
nasm bootload.asm

#Puts bootload into sector zero.
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc

#Compiles kernel.c
bcc -ansi -c -o kernel.o kernel.c

#Assembles kernel.asm
as86 kernel.asm -o kernel_asm.o

#Links kernel.o with kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o

#Puts kernel into sector three.
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
