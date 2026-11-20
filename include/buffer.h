#ifndef BUFFER_H
#define BUFFER_H

#define BUFFER_LEN 64

struct buffer {
	unsigned char ch[BUFFER_LEN];
	int read, write, len;
};

void init_buffer(struct buffer *b);

unsigned char read_buffer(struct buffer * b);
void write_buffer(struct buffer * b, unsigned char ch);

char buffer_empty(struct buffer *b);
char buffer_full(struct buffer *b);

#endif
