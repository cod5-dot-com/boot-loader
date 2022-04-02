#ifndef FAT32_H
#define FAT32_H 1


int fat32_read_block(char *file, char *blk, int start);
int fat32_write_block(char *file, char *blk, int start);
int fat32_set_mbr(char *mbr, int start, int length); 
int fat32_set_vbr_sect(char *blk, int start, int length); 
int fat32_set_fsinfo(char *blk); 
int fat32_format(char *file, int partid, char *name); 
Buf *fat32_list(char *path); 

extern char *fat32_error;
#endif

