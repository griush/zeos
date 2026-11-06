#include <asm.h>

.globl errno

ENTRY(exit)
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
 
	leal 8(%ebp), %ebx
	movl $1, %eax          /* sys_exit */
	int $0x93
 
	/* does not return */
	popl %ebx
	popl %ebp
	ret

ENTRY(fork)
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
 
	leal 8(%ebp), %ebx
	movl $2, %eax          /* sys_fork */
	int $0x93
 
	popl %ebx
	cmpl $0, %eax
	jge fork_end
	negl %eax
	movl %eax, errno
	movl $-1, %eax
fork_end:
	popl %ebp
	ret

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
	pushl %ebx
 
	leal 8(%ebp), %ebx
	movl $20, %eax          /* sys_getpid */
	int $0x93
 
	popl %ebx
	cmpl $0, %eax
	jge getpid_end
	negl %eax
	movl %eax, errno
	movl $-1, %eax
getpid_end:
	popl %ebp
	ret
