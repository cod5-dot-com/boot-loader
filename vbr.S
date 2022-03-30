#           27 march MMXXII PUBLIC DOMAIN by Jean-Marc Lienher
#           The author disclaims copyright to this source code.

# VBR is loaded by the MBR at address 0x0000:7C00
# %dx contains the BIOS disk id
# %ebx contains the LBA address os the VBR
# It loads IO.SYS

.code16
.text
.org 0
.global _start

_start:
	jmp bypass

public_domain:
.ascii "Public Domain Operating System"

.org 0x40
bypass:
	movl $0x83, %ebx
	movb $0x0E, %ah	 	# print V
	movb $'V', %al
	int $0x10

	movw 0x7C00, %sp	# set stack pointer to where we've been loaded
	xorw %ax, %ax
	movw %ax, %ss
	pushl %ebx
	pushl %edx		# save dx value passed by MBR
	movw %ax, %es
	movw %ax, %ds

move_vbr:			# move VBR to 0x0000:0600
	cli
	movw $0x7C00, %si
	movw $0x0600, %di
	movw $512, %cx
	cld
	rep movsb

	sti
	movb $0x0E, %ah	 	# print P
	movb $'B', %al
	int $0x10

	movw $0, ($0x7C00 + (signature - _start)) 	# clear signature
	movw $0, ($0x0600 + (signature - _start)) 	# clear signature
	
	ljmp  $0, $(0x0600 + after_move - _start)

never_here:
	movb $0x0E, %ah		
	movb $'!', %al  	# error
	int $0x10
fail:
	hlt
	jmp fail

after_move:
	movb $0x0E, %ah	 	# R
	movb $'R', %al
	int $0x10
boot:
	movb $' ', %al
	call putc
	popl %edx		# dx value passed by BIOS
	popl %ebx		# ebx value passed by BIOS


	jmp read_io_sys

print_ebx:
	pushal	
	movb $32, %cl
hexlba:
	pushw %si
	movw %sp, %si
	movl %ebx, %eax
	popw %si
	subb $4, %cl
	shrl %cl, %eax
	andw $0x0F, %ax
	addw $hex, %ax
	movw %ax, %si
	movb 0x0600(%si), %al
	call putc

	cmpb $0, %cl
	jnz hexlba
	movb $' ', %al
	call putc
	popal 
	ret


read_io_sys:	# Volume Boot Sector
	movw $0, ($0x7C00 + (signature - _start)) 	# clear signature
	movb $',', %al
	call putc
	call print_ebx

#	movw $0x8, %cx
#wait1:
#	hlt
#	subw $1, %cx
#	jnz wait1

	movb $0x10, (0x0600 + lba_packet_size)
	movb $0x0, (0x0600 + lba_packet_reserved)
	movw $0x0001, (0x0600 + lba_packet_sectors)
	movw $0x7C00, (0x0600 + lba_packet_offset)
	movw $0x0, (0x0600 + lba_packet_segment)
	movl %ebx, (0x0600 + lba_packet_lba)
	movl $0x0, (0x0600 + lba_packet_padding)

	movw $3, %cx
try_read:
	movw $0, %ax
	movw %ax, %ds
	movb $0x42, %ah			# read block, %dx contains disk id
	movw $lba_packet + 0x600, %si
	int $0x13
	jnc read_done 
	movb $0, %ah			# reset disk controller
	int $0x13
	loop try_read

cant_read_lba:
	movb $'#', %al
	call putc
io_error:
	hlt
	jmp io_error

read_done:
	pushl %ebx		# ebx value passed by BIOS
	pushl %edx		# dx value passed by BIOS
	movw (0x7C00 + 510), %ax 	# read signature
	cmpw $0xAA55, %ax
	je sign_ok

go_back:
	movw $10, %cx
del:				# delete previous printed data
	movb $8, %al
	call putc
	subw $1, %cx
	jnz del
	
	popl %edx		# dx value passed by BIOS
	popl %ebx		# ebx value passed by BIOS
	addl $0x1, %ebx
	jmp read_io_sys		# read next blok


sign_ok:			# ok we've found IO.SYS
	movb $' ', %al
	call putc
	movb $'L', %al
	call putc
	movb $'o', %al
	call putc
	movb $'a', %al
	call putc
	movb $'d', %al
	call putc
	movb $0x0D, %al
	call putc
	movb $0x0A, %al
	call putc

	movw $0x10, %cx
wait:
	hlt
	subw $1, %cx
	jnz wait

						# %dx must contain the disk BIOS id 
	movl (0x0600 + lba_packet_lba), %ebx 	# pass lba of the io.sys to io.sys		
	ljmp $0x0000, $0x7C00

never_goes_here:
	call halt

putc:				# print %al char to the console
	movb $0x0E, %ah
	int $0x10
	ret

reboot:
	ljmp $0xFFFF, $0x0000	# cold reboot

reload:
	int $0x19		# jump to first sector of the disk

halt:
	hlt
	movb $'.', %al
	call putc
	jmp halt



hex:
.ascii "0123456789ABCDEF"

.p2align 4,0
lba_packet:
lba_packet_size:
.byte 0x10
lba_packet_reserved:
.byte 0x0
lba_packet_sectors:
.word 0x0001
lba_packet_offset:
.word 0x7C00
lba_packet_segment:
.word 0x0
lba_packet_lba:
.long 0
lba_packet_padding:
.long 0

signature:
.org 510
.byte 0x55, 0xAA

