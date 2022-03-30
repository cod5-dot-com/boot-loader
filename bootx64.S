//                      26 march MMXXII PUBLIC DOMAIN
//           The author disclaims copyright to this source code.
// https://developer.ibm.com/articles/l-gas-nasm/
// https://www.dreamincode.net/forums/topic/350898-converting-gas-to-nasm/
// https://docs.microsoft.com/en-us/windows/win32/debug/pe-format?redirectedfrom=MSDN#section-flags
// https://kirux-bootloaderpart3.blogspot.com/2014/02/writing-bootloader-in-gnu-gas-assembler.html
#https://dox.ipxe.org/UefiSpec_8h_source.html#l01953

 
.code64
.text
#.org 0x200000
.org 0x0
.global _start 

_start:
dos:
    	.long 0x00005a4d       
    	.rept 14 .long 0 
	.endr
    	.long 0x00000080
    	.rept 16 .long 0 
	.endr

pecoff:
	.byte 'P','E',0,0      
    	.word 0x8664       # Machine IMAGE_FILE_MACHINE_AMD64
    	.word 3            # NumberOfSections
    	.long 0x5cba52f6   # TimeDateStamp
	.long 0		# PointerToSymbolTable
	.long 0		# NumberOfSymbols
	.word oend - oheader      # SizeOfOptionalHeader
    	.word 0x202e       # Characteristics

oheader:
    	.word 0x020b   	# PE32+
	.byte 0		# MajorLinkerVersion
	.byte 0		# MinorLinkerVersion
    	.long codesize     # SizeOfCode  
  	.long datasize     # SizeOfInitilizedData
    	.long 0            # SizeOfUninitializedDta
    	.long 4096         # AddressOfEntryPoint
    	.long 4096         # BaseOfCode

    	.quad 0x200000     # ImageBase
    	.long 4096         # SectionAlignment
    	.long 4096         # FileAlignment
	.word 0		# MajorOperatingSystemVersion
	.word 0		# MinorOperatingSystemVersion
	.word 0		# MajorImageVersion
	.word 0		# MinorImageVersion	
	.word 0		# MajorSubsystemVersion
	.word 0		# MinorSubsystemVersion
	.long 0		# Win32VersionValue

    	.long 4096 + datasize + codesize      # SizeOfImage
    	.long 4096         # SizeOfHeaders
    	.long 0            # Checksum
	.word 0x000A	# Subsystem
	.word 0x0040	# DllCharateristics

    	.quad 0x10000      # SizeOfStackReserve
    	.quad 0x10000      # SizeOfStackCommit
    	.quad 0x10000      # SizeOfHeapReserve
    	.quad 0            # SizeOfHeapCommit
    	.long 0            # LOaderFlags
    	.long 0x10         # NumberOfRvaAndSizes

dirs:
    	.rept 5 .quad 0    # unused
	.endr
			# BaseRelocationTable
    	.long 0x004000     # VirtualAddress
    	.long 0      	# Size

        .rept 10 .quad 0   # unused
	.endr
oend:

sects:
_1:
    	.byte '.','t','e','x','t',0,0,0     # Name
    	.long  codesize    # VirtualSize
    	.long  4096        # VirtualAddress   
    	.long  codesize    # SizeOfRawData
    	.long  4096        # PointerToRawData
    	.long  0           # PointerToRelocations 
	.long  0		# PointerToLineNumbers
    	.word  0           # NumberOfRelocations
	.word  0		# NumberOfLineNumbers
    	.long  0x60000020  # Characteristics IMAGE_SCN_CNT_CODE
			# IMAGE_SCN_MEM_EXECUTE IMAGE_SCN_MEM_READ

_2:
        .byte  '.','d','a','t','a',0,0,0
        .long  datasize     
        .long  4096 + codesize #
        .long  datasize
        .long  4096 + codesize #
        .long  0
        .long  0
        .word  0
        .word  0
        .long  0xC0000040    # IMAGE_SCN_MEM_READ IMAGE_SCN_MEM_WRITE  
			  # IMAGE_SCN_CNT_INITIALIZED_DATA


_3:
    	.byte  '.','r','e','l','o','c',0,0
    	.long  0   
    	.long  0 
    	.long  0
    	.long  0 
    	.long  0
    	.long  0
    	.word  0
   	.word  0
    	.long  0x02000040 	# IMAGE_SCN_CNT_INITIALIZED_DATA IMAGE_SCN_MEM_DISCARDABLE
endofhead:
	.p2align 12,0

 
main:
	subq $0x28, %rsp  # 32 byte of shadow space + 8 byte to align stack
	movq %rcx, ImageHandle(%rip)
	movq %rdx, SystemTable(%rip)

go:
	movq SystemTable(%rip), %rdx	

     	# Parameters
	movq 64(%rdx), %rcx 	# EFI_SYSTEM_TABLE_CONOUT  
	leaq hello(%rip), %rdx
	movq $0, %r8
	movq $0, %r9
	movq %rax, 0x20(%rsp)

	movq $0, %rax		# EFI_SUCCESS
	call *8(%rcx)		# EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_OUTPUTSTRING  
	
	jmp go
os:
	addq $0x28, %rsp
	ret

	.p2align 12,0
endcodes:

.set codesize, endcodes - main 

datastart:
_EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID:
	.byte 0xde, 0xa9, 0x42, 0x90 
	.byte 0xdc, 0x23, 0x38, 0x4a
        .byte 0x96, 0xfb, 0x7a, 0xde
	.byte 0xd0, 0x80, 0x51, 0x6a

hello:  	.byte 'H',0,'e',0,'l',0,'l',0,'o',0,13,0,10,0,0,0
ImageHandle: .quad 0
SystemTable: .quad 0  
dataend:
	.rept (4096 - (dataend - datastart) % 4096)
	.byte 0
	.endr
dataends:

.set datasize, dataends - datastart



