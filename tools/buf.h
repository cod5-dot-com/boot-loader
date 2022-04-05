#ifndef BUF_H
#define BUF_H 1


struct Buf_ {
	int size;
	int len;
	char *buf;
};

#define Buf struct Buf_

Buf *buf_new(int size); 
void buf_dispose(Buf *self); 
char *buf_getstr(Buf *self); 
void buf_addstr(Buf *self, char *data); 
void buf_addhex8(Buf *self, int data); 
void buf_addhex16(Buf *self, int data); 
void buf_addhex32(Buf *self, int data); 
void buf_addint(Buf *self, int data); 
void buf_add8(Buf *self, int data); 
void buf_add16(Buf *self, int data); 
void buf_add32(Buf *self, int data); 
void buf_add_utf8(Buf *self, int data); 
void buf_add_utf16(Buf *self, int data); 
int buf_utf8tocp(char *b, int len, int *code_point);
int buf_utf16tocp(char *b, int len, int *code_point);
#endif

