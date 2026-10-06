#include <asm.h>

.globl errno

ENTRY(write)
	pushl %ebp
	movl %esp, %ebp

	pushl %edx
	pushl %ecx
	pushl %ebx

	movl 8(%ebp), %edx // fd
	movl 12(%ebp), %ecx // buffer
	movl 16(%ebp), %ebx // size
	
	movl $4, %eax // 4 for sys_write

	pushl $sys_write_return // save return address
	pushl %ebp
	movl %esp, %ebp
	sysenter

sys_write_return:
	pop %ebp
	addl $4, %esp
	
	popl %ebx
	popl %ecx
	popl %edx

	cmpl $0, %eax
	jge sys_write_end
	negl %eax
	movl %eax, errno
	movl $-1, %eax
sys_write_end:
	popl %ebp
	ret
	
ENTRY(gettime)
	pushl %ebp
	movl %esp, %ebp

	movl $10, %eax // 4 for sys_gettime

	pushl $sys_gettime_return // save return address
	pushl %ebp
	movl %esp, %ebp
	sysenter

sys_gettime_return:
	pop %ebp
	addl $4, %esp

	cmpl $0, %eax
	jge sys_gettime_end
	negl %eax
	movl %eax, errno
	movl $-1, %eax
sys_gettime_end:
	popl %ebp
	ret
	
