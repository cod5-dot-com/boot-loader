
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tools/buf.c"
#include "tools/bin2hex.c"
#include "tools/mkvhd.c"
#include "tools/fat32.c"
#include "tools/format.c"
#include "tools/ls.c"

void run(char *a, char *p, int(*f)(), int argc, char *argv[])
{
	if (!strcmp(a, p)) {
		exit(f(argc -1, argv + 1));
	}
}

int main(int argc, char *argv[]) 
{
	if (argc > 1) {
		run(argv[1], "bin2hex", bin2hex_main, argc, argv);	
		run(argv[1], "mkvhd", mkvhd_main, argc, argv);	
		run(argv[1], "format", format_main, argc, argv);	
		run(argv[1], "ls", ls_main, argc, argv);	
	}
	printf("Not found.");
	return 0;
}

