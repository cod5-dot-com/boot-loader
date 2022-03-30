/*
 *                          cod5.com computer
 *
 *                      17 may MMXXI PUBLIC DOMAIN
 *           The author disclaims copyright to this source code.
 *
 *
 */

#include <stdio.h>

static char hex[] = "0123456789ABCDEF";
int writeOut(FILE *out, int c)
{
	fwrite("0x", 1, 2, out); 
	fwrite(hex + ((c >> 4) & 0xF), 1, 1, out); 
	fwrite(hex + (c & 0xF), 1, 1, out); 
	return 0;
}

int main (int argc, char *argv[])
{
	FILE *in;
	FILE *out;
	int n = 0;
	char a[8];
	int p;
	int c;

	if (argc < 3) {
		printf("USAGE: %s  infile outfile\n", argv[0]);
		exit(-1);
	}	
	in = fopen(argv[1], "rb");
	out = fopen(argv[2], "w+b");
	p = 0;
	while (fread(&c, 1, 1, in) == 1) {
		if (n > 0) {
			fwrite(", ", 1, 2, out); 
			if (p == 8) {
				p = 0;
				fwrite(" //", 1, 3, out); 
				fwrite(a, 1, 8, out); 
				fwrite("\n", 1, 1, out); 
			}
		}
		writeOut(out, c);
		if ((c >= 0x20))  {
			a[p] = c;
		} else {
			a[p] = '.';
		}
		p++;
		n++;
			
	}
	if (p > 0) {
		while (p < 8) {
			a[p] = ' ';
			p++;
		}
		fwrite(" //", 1, 3, out); 
		fwrite(a, 1, 8, out); 
		fwrite("\n", 1, 1, out); 
	}	
	fclose(in);
	fclose(out);
	return 0;
}


