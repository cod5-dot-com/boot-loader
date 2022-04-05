
#include "tools/buf.h"


Buf *buf_new(int size)
{
	Buf *self;
	if (size < 8) {
		size = 8;
	}
	self = malloc(sizeof(Buf));
	self->buf = malloc(size);
	self->size = size;
	self->len = 0;
	return self;
}
 
void buf_dispose(Buf *self) 
{
	free(self->buf);
	free(self);
}

char *buf_getstr(Buf *self) 
{
	if (self->len < self->size) {
		self->buf[self->len] = 0;
	} else {
		buf_add8(self, 0); 
		self->len--;
	}
	return self->buf;
}

void buf_addstr(Buf *self, char *data) 
{
	while (*data) {
		buf_add8(self, *data);
		data++;
	}
}

void buf_addhex8(Buf *self, int data)
{
	char *hex;
	hex = "0123456789ABCDEF";

	buf_add8(self, hex[(data >> 4) & 0x0F]);
	buf_add8(self, hex[data & 0x0F]);
}
 
void buf_addhex16(Buf *self, int data)
{
	buf_addhex8(self, (data >> 8) & 0xFF);
	buf_addhex8(self, data & 0xFF);
}

void buf_addhex32(Buf *self, int data)
{
	buf_addhex16(self, (data >> 16) & 0xFFFF);
	buf_addhex16(self, data & 0xFFFF);
}

void buf_addint(Buf *self, int data) 
{
	int d;
	char b[64];
	int i;
	int c;

	i = 0;
	d = data;
	if (data < 0) {
		buf_add8(self, '-');
		d = -data;
		if (d == data) {
			// d == 0x80000000
			d++;	// -2147483648 + 1
			// d == 0x80000001
			d = -d; //  2147483647
			// d == 0x7FFFFFFF
			c = d % 10 + 1; // c == 7 + 1
			data /= 10;
			if (c == 10) {
				b[i] = '0';
				data++; // add carry
			} else {
				b[i] = '0' + c;
			}
			i++;	
		}
	}
	while (data > 0) {
		b[i] = '0' + (data % 10);
		i++;	
		data /= 10;
	}
	while (i > 0) {
		i--;
		buf_add8(self, b[i]);
	}
}

void buf_add8(Buf *self, int data) 
{
	char *n;
	if (self->len >= self->size) {
		self->size += 512;	
		n = malloc(self->size);
		memcpy(n, self->buf, self->len);
		free(self->buf);
		self->buf = n;
	}
	self->buf[self->len] = data;
	self->len++;
}

void buf_add16(Buf *self, int data) 
{
	buf_add8(self, data & 0xFF);
	buf_add8(self, (data >> 8) & 0xFF);
}

void buf_add32(Buf *self, int data)
{
	buf_add16(self, data & 0xFFFF);
	buf_add16(self, (data >> 16) & 0xFFFF);
}

int buf_utf8tocp(char *b, int len, int *code_point)
{
	int u;
	int i;
	int v;
	i = 0;
	u = 0;
	v = 0;
	if (len < 1) {
		return 0;
	}
	{
		v = b[i];
		if (v < 0x80) {
			u = v;
		} else if (v < 0xC0) {
			// error...
			return -1;
		} else if (v < 0xE0) {
			if (len < 2) {
				return -1;
			}
			u = (v & 0x1F) << 6;
			i++;
			u |= b[i] & 0x3F;
		} else if (v < 0xF0) {
			if (len < 3) {
				return -1;
			}
			u = (v & 0x0F) << 12;
			i++;
			u |= (b[i] & 0x3F) << 6;
			i++;
			u |= b[i] & 0x3F;
		} else if (v < 0xF8) {
			if (len < 4) {
				return -1;
			}
			u = (v & 0x07) << 18;
			i++;
			u |= (b[i] & 0x3F) << 12;
			i++;
			u |= (b[i] & 0x3F) << 6;
			i++;
			u |= b[i] & 0x3F;
		} else if (v < 0xFC) {
			if (len < 5) {
				return -1;
			}
			u = (v & 0x03) << 24;
			i++;
			u |= (b[i] & 0x3F) << 18;
			i++;
			u |= (b[i] & 0x3F) << 12;
			i++;
			u |= (b[i] & 0x3F) << 6;
			i++;
			u |= b[i] & 0x3F;
		} else {
			if (len < 6) {
				return -1;
			}
			u = (v & 0x03) << 30;
			i++;
			u |= (b[i] & 0x3F) << 24;
			i++;
			u |= (b[i] & 0x3F) << 18;
			i++;
			u |= (b[i] & 0x3F) << 12;
			i++;
			u |= (b[i] & 0x3F) << 6;
			i++;
			u |= b[i] & 0x3F;
		}
		i++;
	}
	*code_point = u;
	return i;
}

void buf_add_utf8(Buf *self, int data) 
{
	data &= 0xFFFFFFFF;

	if (data < 0) {
		buf_add8(self, ((data >> 30) & 3) | 252); // 2^30
		buf_add8(self, ((data >> 24) & 63) | 128); // 2^24
		buf_add8(self, ((data >> 18) & 63) | 128);   // 2^18
		buf_add8(self, ((data >> 12) & 63) | 128);    // 2^12
		buf_add8(self, ((data >> 6) & 63) | 128);     // 2^6
		buf_add8(self, ((data) & 63) | 128);
	} else if (data < 0x80) {
		buf_add8(self, ((data) & 127));
	} else if (data < 0x800) {
		buf_add8(self, ((data >> 6) & 31) | 192);
		buf_add8(self, ((data) & 63) | 128);
	} else if (data < 0x10000) {
		buf_add8(self, ((data >> 12) & 15) | 224);
		buf_add8(self, ((data >> 6) & 63) | 128);
		buf_add8(self, ((data) & 63) | 128);
	} else if (data < 0x200000) {
		buf_add8(self, ((data >> 18) & 7) | 240);
		buf_add8(self, ((data >> 12) & 63) | 128);
		buf_add8(self, ((data >> 6) & 63) | 128);
		buf_add8(self, ((data) & 63) | 128);
	} else if (data < 0x4000000) {
		buf_add8(self, ((data >> 24) & 3) | 248);
		buf_add8(self, ((data >> 18) & 63) | 128);
		buf_add8(self, ((data >> 12) & 63) | 128);
		buf_add8(self, ((data >> 6) & 63) | 128);
		buf_add8(self, ((data) & 63) | 128);
	} else {
		buf_add8(self, ((data >> 30) & 3) | 252); // 2^30
		buf_add8(self, ((data >> 24) & 63) | 128); // 2^24
		buf_add8(self, ((data >> 18) & 63) | 128);   // 2^18
		buf_add8(self, ((data >> 12) & 63) | 128);    // 2^12
		buf_add8(self, ((data >> 6) & 63) | 128);     // 2^6
		buf_add8(self, ((data) & 63) | 128);
	}
}

int buf_utf16tocp(char *b, int len, int *code_point)
{
	int data;
	int d2;
	if (len < 2) {
		return -1;
	}
	data = b[0] + (b[1] << 8);
	if (data < 0xD800 || (data > 0xDFFF && data < 0x10000)) {
		*code_point = data;
		return 2;
	}

	if (len < 4) {
		*code_point = data;
		return 2;
	}
	data = (data - 0xD800) << 10;
	d2 = b[2] + (b[3] << 8);
	data +=  d2 - 0xDC00;
}

void buf_add_utf16(Buf *self, int data)
{
	if (data < 0xD800 || (data > 0xDFFF && data < 0x10000)) {
		buf_add16(self, data);
		return;
	}

	data -= 0x010000;
	buf_add16(self, ((data & 0xFFC00) >> 10) + 0xD800);
	buf_add16(self, (data & 0x003FF) + 0xDC00);
}
 


