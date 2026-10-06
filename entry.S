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
      movl 0x2C(%esp), %eax     /* error code */
      movl 0x30(%esp), %edx     /* faulting EIP */
      movl %cr2, %ecx           /* faulting address */
      pushl %eax                /* arg 3: error */
      pushl %ecx                /* arg 2: cr2 */
      pushl %edx                /* arg 1: eip */
      call pagefault_routine
      RESTORE_ALL
      addl $4, %esp /* Limpia el error code de 4 bytes si llegara a retornar */
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

ENTRY(syscall_handler_sysenter)
	push $__USER_DS
	push %EBP // User stack address
	pushfl
	push $__USER_CS
	push 4(%EBP) // User return address
	SAVE_ALL
	cmpl $0, %EAX
	jl sysenter_err
	cmpl $MAX_SYSCALL, %EAX
	jg sysenter_err
	call *sys_call_table(, %EAX, 0x04)
	jmp sysenter_fin
sysenter_err:
	movl $-ENOSYS, %EAX
sysenter_fin:
	movl %EAX, 0x18(%ESP)
	RESTORE_ALL
	movl (%ESP), %EDX // Return address
	movl 12(%ESP), %ECX // User stack address
	sti // Enable interrupts again
	sysexit

ENTRY(writeMSR)
	push %ebp
	movl %esp, %ebp
	movl 8(%ebp), %ecx
	movl $0, %edx
	movl 12(%ebp), %eax
	wrmsr
	popl %ebp
	ret
