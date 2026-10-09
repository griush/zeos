/*
 * entry.S - Entry point to system mode from user mode
 */

#include <asm.h>
#include <segment.h>
#include <errno.h>


/**************************************************/
/**** Save & Restore ******************************/
/**                                              **/
/** When we change to privilege level 0 (kernel) **/
/** (through an interrupt, a system call, an     **/
/** exception ...) we must save the state of the **/
/** currently running task (save).               **/
/**                                              **/
/** Stack layout in 'systemCall':                **/
/**                                              **/
/**   0(%esp) - %edx    \                        **/
/**   4(%esp) - %ecx     |                       **/
/**   8(%esp) - %ebx     |                       **/
/**   C(%esp) - %esi     | Register saved        **/
/**  10(%esp) - %edi     |  by 'save'            **/
/**  14(%esp) - %ebp     |                       **/
/**  18(%esp) - %eax     |                       **/
/**  1C(%esp) - %ds      |                       **/
/**  20(%esp) - %es      |                       **/
/**  24(%esp) - %fs      |                       **/
/**  28(%esp) - %gs     /                        **/
/**  2C(%esp) - %eip    \                        **/
/**  30(%esp) - %cs      |                       **/
/**  34(%esp) - %eflags  |  Return context saved **/
/**  38(%esp) - %oldesp  |   by the processor.   **/
/**  3C(%esp) - %oldss  /                        **/
/**                                              **/
/**************************************************/

#define SAVE_ALL \
      pushl %gs; \
      pushl %fs; \
      pushl %es; \
      pushl %ds; \
      pushl %eax; \
      pushl %ebp; \
      pushl %edi; \
      pushl %esi; \
      pushl %ebx; \
      pushl %ecx; \
      pushl %edx; \
      movl $__KERNEL_DS, %edx;    \
      movl %edx, %ds;           \
      movl %edx, %es;

#define RESTORE_ALL \
      popl %edx; \
      popl %ecx; \
      popl %ebx; \
      popl %esi; \
      popl %edi; \
      popl %ebp; \
      popl %eax; \
      popl %ds; \
      popl %es; \
      popl %fs; \
      popl %gs;

#define EOI \
      movb $0x20, %al; \
      outb %al, $0x20;

ENTRY(pagefault_handler)
      SAVE_ALL
      movl %cr2, %ecx           /* faulting address */
      pushl %ecx                /* arg 2: cr2 */
      pushl %esp                /* arg 1: pointer to struct sys_stack */
      call pagefault_routine
      addl $8, %esp             /* clean args */
      RESTORE_ALL
      addl $4, %esp             /* discard CPU-pushed error code */
      iret

ENTRY(timer_handler)
      SAVE_ALL
      EOI
      call timer_routine
      RESTORE_ALL
      iret

ENTRY(keyboard_handler)
      SAVE_ALL
      EOI
      call keyboard_routine
      RESTORE_ALL
      iret

ENTRY(system_call_handler)
	SAVE_ALL
	cmpl $0, %eax
	jl sys_err
	cmpl $MAX_SYSCALL, %eax
	jge sys_err
	pushl %ebx                      /* pointer to params in the stack */
	call *sys_call_table(, %eax, 4)
	addl $4, %esp
	jmp sys_fin
sys_err:
	movl $-ENOSYS, %eax
sys_fin:
	movl %eax, 0x18(%esp)
	RESTORE_ALL
	iret
