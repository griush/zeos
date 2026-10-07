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
