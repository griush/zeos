#include "include/buffer.h"

void init_buffer(struct buffer *b)
{
	b->read = b->write = 0;
	for (int i = 0; i < BUFFER_LEN; i++) {
		b->ch[i] = 0;
	}
}

unsigned char read_buffer(struct buffer * b)
{
	if (buffer_empty(b))
		return 0;

	b->len--;
	unsigned char c = b->ch[b->read];
	b->read = (b->read + 1) % BUFFER_LEN;
	return c;
}

void write_buffer(struct buffer * b, unsigned char c)
{
	if (buffer_full(b))
		return;

	b->ch[b->write] = c;
	b->write = (b->write + 1) % BUFFER_LEN;
	b->len++;
}

char buffer_empty(struct buffer *b)
{
	return b->len == 0;
}

char buffer_full(struct buffer *b)
{
	return b->len == BUFFER_LEN;
}
