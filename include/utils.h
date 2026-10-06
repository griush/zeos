#ifndef UTILS_H
#define UTILS_H

extern int zeos_ticks;

void copy_data(void *start, void *dest, int size);
int copy_from_user(void *start, void *dest, int size);
int copy_to_user(void *start, void *dest, int size);

#define VERIFY_READ	0
#define VERIFY_WRITE	1
int access_ok(int type, const void *addr, unsigned long size);

void hex_to_str(unsigned int val, char *buf);

#endif
