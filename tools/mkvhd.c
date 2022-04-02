/*
 *                          cod5.com computer
 *
 *                      30 march MMXXII PUBLIC DOMAIN
 *           The author disclaims copyright to this source code.
 *
 *
 */

#include "mbr.h"
#include "vbr.h"
#include "tools/vhd.h"
#include "tools/fat32.h"
#define VBR_OFFSET 4

int mkvhd_pad(FILE *out, int c)
{
	char b[512];
	int n;
	memset(b, 0, 512);
	for (n = 0; n < c; n++) {
		fwrite(b, 512, 1, out); 
	}
	return c;	
}

// https://github.com/libyal/libvhdi/blob/main/documentation/Virtual%20Hard%20Disk%20(VHD)%20image%20format.asciidoc

void mkvhd_trail(char *blk, int s) {
	char *p;
	int i;
	int v;
	int checksum;
	int counter;
	int total_sectors;
	int cylinders;
	int heads;
	int sectors_per_tr;
	int cylinder_x_heads;
	total_sectors = s;
	
	p = blk + 36;
	*p = 'c';
	p++;
	*p = 'o';
	p++;
	*p = 'd';
	p++;
	*p = '5';
	p = blk + 40; // disk size high
	v = 0;  
	for (i = 0; i < 32; i += 8) {
		*p = 0xFF & (v >> (24 - i)); // big endian
	 	p++;
	}
	p = blk + 44; // disk size low 
	v = s * 512;  
	for (i = 0; i < 32; i += 8) {
		*p = 0xFF & (v >> (24 - i)); // big endian
	 	p++;
	}

	p = blk + 48; // data size high
	v = 0;  
	for (i = 0; i < 32; i += 8) {
		*p = 0xFF & (v >> (24 - i)); // big endian
	 	p++;
	}
	p = blk + 52; // disk size low 
	v = s * 512;  
	for (i = 0; i < 32; i += 8) {
		*p = 0xFF & (v >> (24 - i)); // big endian
	 	p++;
	}

	if (total_sectors > 65535 * 16 * 255) {
		total_sectors = 65535 * 16 * 255;
	}
	if (total_sectors > (65535 * 16 * 63)) {
		sectors_per_tr = 255;
		heads = 16;
		cylinder_x_heads = total_sectors / sectors_per_tr;
	} else {
		sectors_per_tr = 17;
		cylinder_x_heads = total_sectors / sectors_per_tr;
		heads = (cylinder_x_heads + 1023) / 1024;
		if (heads < 4) {
			heads = 4;
		}
		if (cylinder_x_heads >= (heads * 1024) || heads > 16) {
			sectors_per_tr = 31;
			heads = 16;
			cylinder_x_heads = total_sectors / sectors_per_tr;
		}	
		if (cylinder_x_heads >= (heads * 1024)) {
			sectors_per_tr = 63;
			heads = 16;
			cylinder_x_heads = total_sectors / sectors_per_tr;
		}	
	}
	cylinders = cylinder_x_heads / heads;
	p = blk + 56; // number of cylinders
	*p = (cylinders >> 8) & 0xFF;
	p++;
	*p = cylinders & 0xFF;
	p++;
	*p = heads; // number of heads
	p++;
	*p = sectors_per_tr; // number of sectors per track

	p = blk + 64; // clear checksum
	*p = 0;
	p++;  
	*p = 0;
	p++;  
	*p = 0;
	p++;  
	*p = 0;

	checksum = 0;
	for (counter = 0; counter < 512; counter++) {
		checksum += blk[counter];
	}
	p = blk + 64; // checksum
	v = ~checksum;  
	for (i = 0; i < 32; i += 8) {
		*p = 0xFF & (v >> (24 - i)); // big endian
	 	p++;
	}
}

int mkvhd_main (int argc, char *argv[])
{
	FILE *out;
	int s;
	int o = 0;
	if (argc < 3) {
		printf("USAGE: %s  size outfile\n", argv[0]);
		exit(-1);
	}
	s = atoi(argv[1]);	
	// FIXME
	s = 0x10000000;
	s += 511;
	s = s / 512;
	out = fopen(argv[2], "w+b");
	if (!out) {
		fprintf(stderr, "cannot open %s\n", argv[2]);
		return -1;
	}
	fat32_set_mbr(mbr, VBR_OFFSET, s - VBR_OFFSET); 
	fwrite(mbr, 1, 512, out); 
	o++;
	o += mkvhd_pad(out, VBR_OFFSET - 1);
	fat32_set_vbr_sect(vbr, VBR_OFFSET, s - VBR_OFFSET); 
	fwrite(vbr, 1, 512, out); 
	o++;
	o += mkvhd_pad(out, (s - o));

	mkvhd_trail(vhd, s);
	fwrite(vhd, 1, 512, out); 
	fclose(out);
	return 0;
}


