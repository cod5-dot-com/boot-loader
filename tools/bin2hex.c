/*
 *                          cod5.com computer
 *
 *                      17 may MMXXI PUBLIC DOMAIN
 *           The author disclaims copyright to this source code.
 *
 *
 */

static char hex[] = "0123456789ABCDEF";
int writeOut(FILE *out, int c)
{
	fwrite("0x", 1, 2, out); 
	fwrite(hex + ((c >> 4) & 0xF), 1, 1, out); 
	fwrite(hex + (c & 0xF), 1, 1, out); 
	return 0;
}

int bin2hex_main (int argc, char *argv[])
{
	FILE *in;
	FILE *out;
	int n = 0;
	char a[8];
	int p;
	char c[4];
	char *vn;

	if (argc < 3) {
		fprintf(stderr, "USAGE: %s  infile outfile\n", argv[0]);
		exit(-1);
	}	
	in = fopen(argv[1], "rb");
	if (!in) {
		fprintf(stderr, "cannot open %s\n", argv[1]);
		exit(-1);
	}
	out = fopen(argv[2], "w+b");
	if (!out) {
		fprintf(stderr, "cannot open %s\n", argv[2]);
		exit(-1);
	}
	fwrite("char ", 1, 5, out); 
	vn = argv[2];
	while (*vn && *vn != '.') {
		fwrite(vn, 1, 1, out); 
		vn++;
	}
	fwrite("[]={\n", 1, 5, out); 
	p = 0;
	while (fread(c, 1, 1, in) == 1) {
		if (n > 0) {
			fwrite(", ", 1, 2, out); 
			if (p == 8) {
				p = 0;
				fwrite(" //", 1, 3, out); 
				fwrite(a, 1, 8, out); 
				fwrite("\n", 1, 1, out); 
			}
		}
		writeOut(out, c[0]);
		if (c[0] >= ' ' && c[0] < 0x7F)  {
			a[p] = c[0];
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
		fwrite("   //", 1, 5, out); 
		fwrite(a, 1, 8, out); 
		fwrite("\n", 1, 1, out); 
	}	
	fwrite("};\n", 1, 3, out); 
	fclose(in);
	fclose(out);
	return 0;
}


