/* some functions of the kernel implemented in assembly */
#include <asm.h>

/* void switch_stack(DWord *save_sp, DWord new_sp) */
ENTRY(switch_stack)
	movl 4(%esp), %eax
	movl %ebp, (%eax)
	movl 8(%esp), %esp
	popl %ebp
	ret

/* void task_switch(union task_union *new) */
ENTRY(task_switch)
	pushl %ebp
	movl %esp, %ebp
	pushl %esi
	pushl %edi
	pushl %ebx
	pushl 8(%ebp)
	call inner_task_switch
	addl $4, %esp
	popl %ebx
	popl %edi
	popl %esi
	popl %ebp
	ret
