#ifndef FAT32_H
#define FAT32_H 1


int fat32_set_mbr(char *mbr, int start, int length) {
	char *p;
	int i;
	p = mbr + 446; // partition table first volume 
	*p = 0x80; // status ACTIVE
	p++; 
	for (i = 0; i < 24; i += 8) {
		*p = 0;// start chs
		p++;
	}
	*p = 0x0C; // type FAT32 with LBA
	p++; 
	for (i = 0; i < 24; i += 8) {
		*p = 0;// end chs
		p++;
	}
	for (i = 0; i < 32; i += 8) {
		*p = 0xFF & (start >> i);//  start lba
	 	p++;
	}
	for (i = 0; i < 32; i += 8) {
		*p = 0xFF & (length >> i);//  length lba
	 	p++;
	}
	return 0;
}

int fat32_set_vbr_sect(char *blk, int start, int length) {
	char *p;
	int i;
	int n;
	p = blk + 0x1C;
	n = start;	// number of hidden sector
	for (i = 0; i < 32; i += 8) {
		*p = 0xFF & (n >> i);
	 	p++;
	}
	
	p = blk + 0x13;
	n = length;	// sector count
	if (n > 0xFFFF) {
		n = 0;
	} else {
		length = 0;
	}
	for (i = 0; i < 16; i += 8) {
		*p = 0xFF & (n >> i);
	 	p++;
	}
	p = blk + 0x1C;
	n = length;	// number of hidden sector
	for (i = 0; i < 32; i += 8) {
		*p = 0xFF & (n >> i);
	 	p++;
	}
	// 0x40 offset
	return 0;
}

int fat32_set_fsinfo(char *blk) {
	char *p;
	int i;
	int v;
	p = blk;
	v = 0x41615252; // lead signature 
	for (i = 0; i < 32; i += 8) {
		*p = 0xFF & (v >> i);
	 	p++;
	}
	p = blk + 0x1E4;
	v = 0x61417272; // signature
	for (i = 0; i < 32; i += 8) {
		*p = 0xFF & (v >> i);
	 	p++;
	}
	v = 0xFFFFFFFF; // free clusters 
	for (i = 0; i < 32; i += 8) {
		*p = 0xFF & (v >> i);
	 	p++;
	}
	v = 0xFFFFFFFF; // start available cluster 
	for (i = 0; i < 32; i += 8) {
		*p = 0xFF & (v >> i);
	 	p++;
	}

	p = blk + 0x1FC;
	v = 0xAA550000;  // trail signature
	for (i = 0; i < 32; i += 8) {
		*p = 0xFF & (v >> i);
	 	p++;
	}
	return 0;
}
#endif

