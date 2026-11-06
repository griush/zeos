/*
 * sched.h - Estructures i macros pel tractament de processos
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#include <list.h>
#include <types.h>
#include <mm_address.h>

#define KERNEL_STACK_SIZE	1024

enum state_t { ST_RUN, ST_READY, ST_BLOCKED };

struct task_struct {
	int PID;			/* Process ID. This MUST be the first field of the struct. */
	int remaining_quantum;          /* quantum that is left in the process */
	int quantum;			/* the qtt in resets to/starts with */
	enum state_t state;
	page_table_entry * dir_pages_baseAddr;

	DWord kernel_esp;

	struct list_head list;
};

union task_union {
	struct task_struct task;
	unsigned long stack[KERNEL_STACK_SIZE];    /* pila de sistema, per procés */
};

extern struct list_head ready_queue;

#define KERNEL_ESP(t)       	(DWord) &(t)->stack[KERNEL_STACK_SIZE]

extern char initial_stack[KERNEL_STACK_SIZE];
#define INITIAL_ESP             (DWord) &initial_stack[KERNEL_STACK_SIZE]

extern struct task_struct *init_task;
extern struct task_struct *idle_task;

/* Inicialitza les dades del proces inicial */
void init_task1(void);

void init_idle(void);

void init_sched(void);

struct task_struct * current();

void schedule(void);

int get_quantum(struct task_struct *t);
void set_quantum(struct task_struct *t, int new_quantum);

/* defined in kernel.S */
void task_switch(union task_union *new);

page_table_entry * get_PT (struct task_struct *t) ;

page_table_entry * get_DIR (struct task_struct *t) ;

#endif  /* __SCHED_H__ */
