
PATH:= $(PATH)

all: ls 

ls: shell
	./sh ls \dev/./d1.vhd:\

run: shell
	./sh mkvhd 41943040  d.vhd
	qemu-system-i386 -drive format=raw,file=d.vhd || echo bad

vbr86: gas86
	as vbr.S -o vbr.o
	dd if=vbr.o of=vbr.bin bs=1 skip=140 count=512
	cp mbr.vhd vbr.vhd
	dd if=vbr.bin of=vbr.vhd bs=512  skip=0 seek=128 count=1
	dd if=mbr.vhd of=vbr.vhd bs=512  skip=129 seek=129 
gas86:
	as mbr.S -o mbr.o
	dd if=mbr.o of=mbr.bin bs=1 skip=140 count=512
	dd if=mbr.bin of=mbr.vhd bs=1  count=446 
	dd if=disk.vhd of=mbr.vhd bs=1  skip=446 seek=446 count=66
	dd if=disk.vhd of=mbr.vhd bs=512  skip=1 seek=1 
#	qemu-system-i386 -drive format=raw,file=mbr.vhd || echo bad

gas32:
	as  bootia32.S -o bootia32.o
	dd if=bootia32.o of=bootia32.efi bs=1 skip=140
	mkdir -p M/EFI/BOOT || mkdir m\efi\boot || echo ...
	cp bootia32.efi M/EFI/BOOT/bootia32.efi
	qemu-system-i386 -bios bios32.fd -drive driver=vvfat,rw=on,dir=M || echo bad


gas64:
	as bootx64.S -o bootx64.o
	dd if=bootx64.o of=bootx64.efi bs=1 skip=140
	mkdir -p M/EFI/BOOT || mkdir m\efi\boot || echo ...
	cp bootx64.efi M/EFI/BOOT/bootx64.efi
	qemu-system-x86_64 -cpu qemu64 -bios OVMF.fd -drive driver=vvfat,rw=on,dir=M || echo bad

x86: 
	nasm -f bin mbr.asm -o mbr.bin
	dd if=mbr.bin of=mbr.vhd bs=1  count=446 
	dd if=disk.vhd of=mbr.vhd bs=1  skip=446 seek=446 count=66
	dd if=disk.vhd of=mbr.vhd bs=512  skip=1 seek=1 
	qemu-system-i386 -drive format=raw,file=mbr.vhd || echo bad

ia32: 
	nasm -f bin bootia32.asm -o bootia32.efi
	mkdir -p M/EFI/BOOT || mkdir m\efi\boot || echo ...
	cp bootia32.efi M/EFI/BOOT/bootia32.efi
	rm M/EFI/BOOT/bootx64.efi || echo ..
	qemu-system-i386 -bios bios32.bin -drive driver=vvfat,rw=on,dir=M || echo bad

x64: 
	nasm -f bin bootx64.asm -o bootx64.efi
	mkdir -p M/EFI/BOOT || mkdir m\efi\boot || echo ...
	cp bootx64.efi M/EFI/BOOT/bootx64.efi
	qemu-system-x86_64 -cpu qemu64 -bios OVMF.fd -drive driver=vvfat,rw=on,dir=M || echo bad

shell: vbr86
	rm -f tools/sh.s
	echo "char mbr[] = {0};" > mbr.h
	echo "char vbr[] = {0};" > vbr.h
	scc -o sh.exe tools/sh.c
	./sh bin2hex mbr.bin mbr.h
	./sh bin2hex vbr.bin vbr.h
	scc -o sh.exe tools/sh.c

clean:
	rm -f sh.exe
	rm -f d.vhd
	rm -f mbr.vhd vbr.vhd
	rm -f *.o *.so *.efi *.bin
	rm -f M/EFI/BOOT/*.efi
	rm -f M/NvVars
