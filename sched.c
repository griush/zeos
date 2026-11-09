/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <hardware.h>

char initial_stack[KERNEL_STACK_SIZE] __attribute__ ((aligned(4096))); // Space for the initial system stack

struct list_head readyqueue;

struct task_struct * init_task = NULL;
struct task_struct * idle_task = NULL;


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
    int task_frame = alloc_frame();
    union task_union *idle_union = (union task_union *) (task_frame << 12);
    
    idle_union->task.PID = 0;
    
    // Crear directorio propio (sólo mapea kernel)
    int page_dir_frame = alloc_frame();
    page_table_entry *dir = (page_table_entry *) (page_dir_frame << 12);
    clear_page_table(dir);
    set_kernel_pages(dir);
    
    idle_union->task.dir_pages_baseAddr = dir;
    idle_union->task.quantum = 10;
    idle_union->task.ticks_left = 10;
    
    // Preparar la pila de sistema para que sea compatible con task_switch
    idle_union->stack[KERNEL_STACK_SIZE - 1] = (unsigned long) cpu_idle; // Dirección de retorno
    idle_union->stack[KERNEL_STACK_SIZE - 2] = 0;                        // Fake EBP
    
    idle_union->task.kernel_esp = (unsigned long *) &(idle_union->stack[KERNEL_STACK_SIZE - 2]);
    
    idle_task = &(idle_union->task);
}

void init_task1(void)
{
    int page_dir_frame = alloc_frame(); // Frame para el directorio de páginas
    page_table_entry *dir = (page_table_entry *) (page_dir_frame << 12);
    
    clear_page_table(dir);
    
    // Configurar mapeos de Kernel y Usuario
    set_kernel_pages(dir);
    set_user_pages(dir);
    
    // Reservar frame para el task_union del proceso Init (PID 1)
    int task_frame = alloc_frame();
    union task_union *init_union = (union task_union *) (task_frame << 12);
    
    init_union->task.PID = 1;
    init_union->task.dir_pages_baseAddr = dir;
    init_union->task.quantum = 10;
    init_union->task.ticks_left = 10;
    
    init_task = &(init_union->task);
    
    // Configurar las estructuras de hardware TSS y MSR para el modo Kernel
    tss.esp0 = (unsigned long) &(init_union->stack[KERNEL_STACK_SIZE]);
    writeMSR(0x175, (unsigned long) tss.esp0);
    
    // Cargar la tabla de páginas del proceso Init en el hardware MMU (CR3)
    set_cr3(dir);
}

void inner_task_switch(union task_union *new) {
    /* 1. Guardar el marco EBP actual en el PCB de current */
    current()->kernel_esp = (unsigned long *) get_ebp();
    
    /* 2. Actualizar TSS y MSR para la pila de sistema de 'new' */
    tss.esp0 = (unsigned long) &(new->stack[KERNEL_STACK_SIZE]);
    writeMSR(0x175, tss.esp0);
    
    /* 3. Cambiar el directorio de páginas (CR3) si es un proceso distinto */
    if (current()->dir_pages_baseAddr != new->task.dir_pages_baseAddr) {
        set_cr3(new->task.dir_pages_baseAddr);
    }
    
    /* 4. Cambiar la pila de sistema activa a la de 'new' */
    set_esp((unsigned long) new->task.kernel_esp);
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

void update_sched_data_rr(void) {
    current()->ticks_left--;
}

int needs_sched_rr(void) {
    if (current()->ticks_left == 0 && !list_empty(&readyqueue)) {
        return 1;
    }
    return 0;
}

void update_process_state_rr(struct task_struct *t, struct list_head *dst_queue) {
    if (t->list.next != NULL && t->list.prev != NULL) {
        list_del(&(t->list));
    }
    if (dst_queue != NULL) {
        list_add_tail(&(t->list), dst_queue);
    }
}

void sched_next_rr(void) {
    struct task_struct *next;
    
    if (!list_empty(&readyqueue)) {
        struct list_head *e = list_first(&readyqueue);
        list_del(e);
        next = list_head_to_task_struct(e);
    } else {
        next = idle_task; /* Seleccionar Idle si no hay tareas listas */
    }
    
    next->ticks_left = get_quantum(next);
    task_switch((union task_union *) next);
}

void schedule(void) {
    update_sched_data_rr();
    if (needs_sched_rr()) {
        update_process_state_rr(current(), &readyqueue);
        sched_next_rr();
    }
}