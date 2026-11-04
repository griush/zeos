/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>
#include <utils.h>
#include <io.h>
#include <mm.h>
#include <mm_address.h>
#include <sched.h>
#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1

int zeos_ticks;

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF;
  if (permissions!=ESCRIPTURA) return -EACCES;
  return 0;
}

int sys_ni_syscall()
{
	return -ENOSYS;
}

static int next_pid = 2;
extern void ret_from_fork(void);

int sys_fork(void)
{
	struct task_struct *parent = current();
	page_table_entry *parent_dir = get_DIR(parent);
	page_table_entry *sys_pt = get_PT(parent);
	unsigned int sys_pt_frame = parent_dir[0].bits.pbase_addr;
	page_table_entry *parent_usr_pt =
		(page_table_entry *)((unsigned int)parent_dir[1].bits.pbase_addr << 12);

	/* 0 = task_union, 1 = dir, 2 = user PT, 3.. = data pages */
	int frames[3 + NUM_PAG_DATA];
	int i, n;

	/* reserve everything, roll back on failure */
	for (n = 0; n < 3 + NUM_PAG_DATA; n++) {
		frames[n] = alloc_frame();
		if (frames[n] == -1) {
			while (--n >= 0) free_frame(frames[n]);
			return -ENOMEM;
		}
	}

	union task_union *child = (union task_union *)(frames[0] << 12);
	page_table_entry *child_dir = (page_table_entry *)(frames[1] << 12);
	page_table_entry *child_usr_pt = (page_table_entry *)(frames[2] << 12);

	set_ss_pag(sys_pt, frames[0], frames[0], 0);
	set_ss_pag(sys_pt, frames[1], frames[1], 0);
	set_ss_pag(sys_pt, frames[2], frames[2], 0);

	/* user page table */
	clear_page_table(child_usr_pt);

	/* code: shared with the parent */
	for (i = 0; i < NUM_PAG_CODE; i++)
		child_usr_pt[NUM_PAG_DATA + i].entry =
			parent_usr_pt[NUM_PAG_DATA + i].entry;

	/* data: private copy */
	for (i = 0; i < NUM_PAG_DATA; i++) {
		int f = frames[3 + i];
		set_ss_pag(child_usr_pt, i, f, 1);

		set_ss_pag(sys_pt, f, f, 0);
		copy_data((void *)((PAG_LOG_INIT_DATA + i) << 12),
		          (void *)(f << 12), PAGE_SIZE);
		del_ss_pag(sys_pt, f);
	}
	set_cr3(parent_dir);                            /* flush TLB */

	/* directory */
	clear_page_table(child_dir);
	set_ss_pag(child_dir, 0, sys_pt_frame, 0);
	set_ss_pag(child_dir, 1, frames[2], 1);

	/* setup PCB */
	copy_data(parent, child, sizeof(union task_union));

	child->task.PID = next_pid++;
	child->task.dir_pages_baseAddr = child_dir;

	child->stack[KERNEL_STACK_SIZE - 10] = 0;                       /* eax = 0 */
	child->stack[KERNEL_STACK_SIZE - 17] = (DWord)ret_from_fork;    /* ret addr */
	child->stack[KERNEL_STACK_SIZE - 18] = 0;                       /* fake ebp */
	child->task.kernel_esp = (DWord)&child->stack[KERNEL_STACK_SIZE - 18];

	/* schedule it */
	list_add_tail(&child->task.list, &ready_queue);

	return child->task.PID;
}

struct write_params {
	int fd;
	char *buffer;
	int size;
};

int sys_write(void *params)
{
	struct write_params p;
	int done = 0;

	if (!access_ok(VERIFY_READ, params, sizeof(p)))
		return -EFAULT;
	copy_from_user(params, &p, sizeof(p));

	int cfd = check_fd(p.fd, ESCRIPTURA);
	if (cfd != 0)
		return cfd;

	if (p.buffer == 0)
		return -EFAULT; /* bad buffer */
	if (p.size < 0)
		return -EINVAL; /* bad write size */
	if (!access_ok(VERIFY_READ, p.buffer, p.size))
		return -EFAULT;

	char s_buff[256];
	while (done < p.size) {
		int n = p.size - done;
		if (n > 256) n = 256;
		copy_from_user(p.buffer + done, s_buff, n);
		sys_write_console(s_buff, n);
		done += n;
	}
	return done;
}

int sys_gettime(void)
{
	return zeos_ticks;
}

int sys_getpid(void)
{
	return current()->PID;
}
