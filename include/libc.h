/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

#ifndef NULL /* already defined for system so this is for user only */
#define NULL ( (void *) 0)
#endif /* NULL */

void itoa(int a, char *b);

int strlen(const char *a);

void perror(void);

/* syscalls
 * defined in syscalls.S
 */
int fork(void);
int write (int fd, const char *buffer, int size);
int gettime(void);
int getpid(void);

#endif  /* __LIBC_H__ */
