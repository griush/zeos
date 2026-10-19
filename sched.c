/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <devices.h>
#include <sched.h>
#include <mm.h>
#include <io.h>
#include <hardware.h>
#include <list.h>

char initial_stack[KERNEL_STACK_SIZE] __attribute__ ((aligned(4096))); // Space for the initial system stack

struct list_head ready_queue;

struct task_struct *init_task;
struct task_struct *idle_task;

void cpu_idle(void)
{
	__sti();
	while(1)
	{
		char *b = "idle process\n";
		sys_write_console(b, 13);
	;
	}
}

void init_idle(void)
{
	// 1)
	int dir = alloc_frame();
	page_table_entry *dir_address = (page_table_entry *)(dir << 12);
	clear_page_table(dir_address);

	// get system PT from init process
	page_table_entry *init_dir = get_DIR(init_task);
	unsigned int sys_pt_frame = init_dir[0].bits.pbase_addr;
	page_table_entry *sys_pt_address = (page_table_entry *)(sys_pt_frame << 12);
	set_ss_pag(dir_address, 0x00, sys_pt_frame, 0);

	// 4) the order should make this go before 2 and 3
	int idle_frame = alloc_frame();
	union task_union *idle_p = (union task_union *)(idle_frame << 12);

	// 2)
	idle_p->task.dir_pages_baseAddr = dir_address;

	// 3)
	idle_p->stack[KERNEL_STACK_SIZE - 1] = (DWord)cpu_idle; // what gets
							      // executed
	idle_p->stack[KERNEL_STACK_SIZE - 2] = NULL; // %ebp
	idle_p->task.kernel_esp = (DWord)(&idle_p->stack[KERNEL_STACK_SIZE - 2]);

	// 5)
	set_ss_pag(sys_pt_address, idle_frame, idle_frame, 0);

	// 6)
	idle_p->task.PID = 0;

	// 7)
	idle_task = &(idle_p->task);
}

void init_task1(void)
{
	// 1)
	int dir = alloc_frame();
	page_table_entry *dir_address = (page_table_entry *)(dir << 12);
	clear_page_table(dir_address);

	int sys_pt = alloc_frame();
	page_table_entry *sys_pt_address = (page_table_entry *)(sys_pt << 12);
	set_kernel_pages(sys_pt_address);

	int usr_pt = alloc_frame();
	page_table_entry *usr_pt_address = (page_table_entry *)(usr_pt << 12);
	set_user_pages(usr_pt_address);

	set_ss_pag(sys_pt_address, dir, dir, 0);
	set_ss_pag(sys_pt_address, sys_pt, sys_pt, 0);
	set_ss_pag(sys_pt_address, usr_pt, usr_pt, 0);

	set_ss_pag(dir_address, 0x00, sys_pt, 0);
	set_ss_pag(dir_address, 0x01, usr_pt, 1);

	// 2)
	int init_frame = alloc_frame();
	union task_union *init_p = (union task_union *)(init_frame << 12);

	// 3)
	set_ss_pag(sys_pt_address, init_frame, init_frame, 0);

	// 4)
	init_p->task.PID = 1;

	// 5)
	tss.esp0 = KERNEL_ESP(init_p);

	// 6)
	init_p->task.dir_pages_baseAddr = dir_address;

	// 7)
	set_cr3(dir_address);

	// 8)
	init_task = &(init_p->task);
}


void init_sched()
{
	INIT_LIST_HEAD(&ready_queue);
}

void switch_stack(DWord *save_sp, DWord new_sp);

/* called from kernel.S */
void inner_task_switch(union task_union* new)
{
	page_table_entry* nd = get_DIR(&new->task);

	/* Update TSS and MSR to make it point to the new stack */
	tss.esp0 = (DWord)&(new->stack[KERNEL_STACK_SIZE]);
	set_cr3(nd);

	// calls into kernel.S to save
	switch_stack(&current()->kernel_esp, new->task.kernel_esp);
}

/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t)
{
       return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t)
{
       return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}

