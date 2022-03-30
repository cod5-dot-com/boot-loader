
#include <stdlib.h>
#include <string.h>

#define main bin2hex_main
#include "./tools/bin2hex.c"
#undef main

int main(int argc, char *argv[]) 
{
	if (argc > 1) {
		switch(argv[1][0]) {
		case 'b':
			if (!strcmp(argv[1], "bin2hex")) return bin2hex_main(argc - 1, argv + 1);
			break;
		} 
			
	}
	return 0;
}

