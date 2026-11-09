#include <asm.h>

.globl errno

ENTRY(write)
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
 
	leal 8(%ebp), %ebx
	movl $4, %eax           /* sys_write */
	int $0x93
 
	popl %ebx
	cmpl $0, %eax
	jge write_end
	negl %eax
	movl %eax, errno
	movl $-1, %eax
write_end:
	popl %ebp
	ret
 
ENTRY(gettime)
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
 
	leal 8(%ebp), %ebx
	movl $10, %eax          /* sys_gettime */
	int $0x93
 
	popl %ebx
	cmpl $0, %eax
	jge gettime_end
	negl %eax
	movl %eax, errno
	movl $-1, %eax
gettime_end:
	popl %ebp
	ret

ENTRY(getpid)
    pushl %ebp
    movl %esp, %ebp
    movl $20, %eax           /* Identificador de la syscall getpid = 20 */
    int $0x80                /* Invocación al kernel por interrupción de software */
    testl %eax, %eax
    jns getpid_ok
    negl %eax
    movl %eax, errno
    movl $-1, %eax
getpid_ok:
    popl %ebp
    ret

ENTRY(fork)
    pushl %ebp
    movl %esp, %ebp
    movl $2, %eax            /* Identificador de la syscall fork = 2 */
    int $0x80
    testl %eax, %eax
    jns fork_ok
    negl %eax
    movl %eax, errno
    movl $-1, %eax
fork_ok:
    popl %ebp
    ret

ENTRY(exit)
    pushl %ebp
    movl %esp, %ebp
    movl $1, %eax            /* Identificador de la syscall exit = 1 */
    int $0x80
    popl %ebp
    ret