#           27 march MMXXII PUBLIC DOMAIN by Jean-Marc Lienher
#           The author disclaims copyright to this source code.

# MBR sector loaded by the BIOS at address 0x0000:7C00
# It copies itself at address 0x0000:0600 and loads the VBR at 0x0000:7C00
# It then jumps to 7C000

.code16
.text
.org 0
.global _start

_start:
	movw (0x7C00 + 510), %ax 	# read signature
	movw $0, (0x7C00 + 510)		# clear signature
	cmpw $0xAA55, %ax
	jz sign_ok

error_sign:
	movb $0x0E, %ah	 	# print $
	movb $'$', %al
	int $0x10
bad_sign:
	hlt
	jmp bad_sign		# infinite loop

sign_ok:
	movb $0x0E, %ah	 	# print M
	movb $'M', %al
	int $0x10

	movw 0x7C00, %sp	# set stack pointer to where we've been loaded
	xorw %ax, %ax
	movw %ax, %ss
	pushw %dx		# save dx value passed by BIOS
	movw %ax, %es
	movw %ax, %ds

move_mbr:
	cli
	movw $0x7C00, %si	# source
	movw $0x0600, %di	# destination
	movw $512, %cx
	cld
	rep movsb

	sti
	movb $'B', %al		# print B
	call putc

	ljmp  $0, $(0x0600 + after_move - _start) 

	movb $'!', %al  # error
	call putc

after_move:
	movb $'R', %al		# print A
	call putc


	movw $0, %bx	# partition index (0 .. 3)
boot:
	popw %dx		# disk number dx value passed by BIOS
check_lba:
	movb $0x41, %ah		# Does BIOS support LBA ?
	movw $0x55AA, %bx
	int $0x13 
	cmpw $0xAA55, %bx
	jz read_vbr

no_lba:				# BIOS is too old for us
	movb $'@', %al
	call putc
lba_error:
	hlt
	jmp lba_error

read_vbr:					# read Volume Boot Sector
	pushaw
	movl (0x0600 + p_start_lba), %eax	# read start address of Volume in the patition table
	movl %eax, (0x0600 + lba_packet_lba)	# put it to the int 0x13 structure
	movw $3, %cx				# max 3 attempts to read form disk
try_read:
	movb $0x42, %ah				# read
	movw $lba_packet + 0x600, %si
	int $0x13
	jnc read_done 
reset_disk:
	movb $0, %ah				# reset
	int $0x13
	loop try_read

cant_read_vbr:
	movb $'#', %al
	call putc
vbr_error:
	hlt
	jmp vbr_error

read_done:
	popaw

	movb $' ', %al	# print "boot\r\n"
	call putc
	movb $'b', %al
	call putc
	movb $'o', %al
	call putc
	movb $'o', %al
	call putc
	movb $'t', %al
	call putc
	movb $0x0D, %al
	call putc
	movb $0x0A, %al
	call putc
	
	movl (0x0600 + p_start_lba), %ebx 	# pass lba of the vbr to the vbr	
						# %dx is preserved too
	ljmp $0x0000, $0x7C00			# jump to VBR

	call halt

putc:						# print the char in %al to the console
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

.p2align 4,0
lba_packet:			# structure for int 0x13 reading
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

.org 440
nt_disk_number:
.long 0x12345678
.word 0

.org  446
partition_table:
p_status:
.byte 0x80	# active
p_start_chs:
.byte 0,0,0
p_type:
.byte 0x0B	# fat32
p_end_chs:
.byte 0,0,0
p_start_lba:
.long 0x0	# this is the block of the begining of the VBR
p_length_lba:
.long 0x0	# size of the Volume

three_other_volumes:
.rept 48 
.byte 0
.endr
 
.org 510
.byte 0x55, 0xAA	# signature

