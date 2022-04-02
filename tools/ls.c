/*
 *                          cod5.com computer
 *
 *                      1 april MMXXII PUBLIC DOMAIN
 *           The author disclaims copyright to this source code.
 *
 *
 */

#include "tools/fat32.h"

int ls_main(int argc, char *argv[])
{
	Buf *b;

	if (argc < 2) {
		fprintf(stderr, "USAGE: %s  /dev./disk.vhd:/path \n", argv[0]);
		exit(-1);
	}
		
	b = fat32_list(argv[1]);
	if (!b) {
		fprintf(stderr, "%s\n", fat32_error);
		return -1;
	}
	puts(buf_getstr(b));
	buf_dispose(b);
	return 0;
}


