/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <hardware.h>

char initial_stack[KERNEL_STACK_SIZE] __attribute__ ((aligned(4096))); // Space for the initial system stack

struct list_head readyqueue;

struct task_struct *list_head_to_task_struct(struct list_head *l) { // CHECK !!!!!!!!!!
	return (struct task_struct *)((unsigned long)l - (unsigned long)&(((struct task_struct *)0)->list));
}


void cpu_idle(void)
{
	__sti();
	while(1)
	{
	;
	}
}

void init_idle (void)
{

}

void init_task1(void)
{
	// Reservar y limpiar el directorio de paginas
	int dir_frame = alloc_frame();
	page_table_entry *dir = (page_table_entry *)(dir_frame << 12);
	clear_page_table(dir);

	// Crear y mapear la tabla de paginas de sistema
	int sys_pt_frame = alloc_frame();
	page_table_entry *sys_pt = (page_table_entry *)(sys_pt_frame << 12);
	clear_page_table(sys_pt);
    set_kernel_pages(sys_pt);

	// dir apunta a la tabla de kernel (R/W=1, Present=1, Supervisor)
	dir.entry = (sys_pt_frame << 12) | 0x3;

	// Crear y mapear la tabla de paginas de usuario
    int usr_pt_frame = alloc_frame();
    page_table_entry *usr_pt = (page_table_entry *)(usr_pt_frame << 12);
    clear_page_table(usr_pt);
    set_user_pages(usr_pt);

	// dir[18] apunta a la tabla de usuario (User=1, R/W=1, Present=1)
    dir[18].entry = (usr_pt_frame << 12) | 0x7;

	// Reservar memoria para el task_union de Init
    int task_frame = alloc_frame();
    union task_union *init_union = (union task_union *)(task_frame << 12);
    init_task = &(init_union->task);

	// Asignar atributos al PCB
    init_task->PID = 1;
    init_task->dir_pages_baseAddr = dir;
    init_task->quantum = 10;
    init_task->pending_ticks = 10;

	// Configurar la TSS y el MSR 0x175 con el tope del system stack
    tss.esp0 = (unsigned long)&(init_union->stack[KERNEL_STACK_SIZE]);
    writeMSR(0x175, tss.esp0);
	
	// Activar el nuevo espacio de direcciones en CR3
    set_cr3(dir);
}


void init_sched(void)
{
	INIT_LIST_HEAD(&readyqueue);
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

