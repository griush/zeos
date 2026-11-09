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

exxtern void ret_from_fork(void);

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

int sys_getpid(void) {
    return current()->PID;
}

int sys_fork(void) {
    /* 1. Reservar un task_union para el proceso hijo */
    int child_frame = alloc_frame();
    if (child_frame < 0) return -ENOMEM;
    
    union task_union *child_union = (union task_union *) (child_frame << 12);
    union task_union *parent_union = (union task_union *) current();
    
    /* 2. Copiar el task_union del padre al hijo */
    copy_data(parent_union, child_union, sizeof(union task_union));
    
    /* 3. Crear directorio de páginas propio para el hijo */
    int child_dir_frame = alloc_frame();
    if (child_dir_frame < 0) {
        free_frame(child_frame);
        return -ENOMEM;
    }
    
    page_table_entry *child_dir = (page_table_entry *) (child_dir_frame << 12);
    clear_page_table(child_dir);
    set_kernel_pages(child_dir); /* Mapeo de kernel compartido */
    
    /* 4. Reservar memoria física para Datos y Pila de usuario del hijo */
    int child_user_frames[NUM_PAG_DATA];
    for (int i = 0; i < NUM_PAG_DATA; i++) {
        child_user_frames[i] = alloc_frame();
        if (child_user_frames[i] < 0) {
            for (int j = 0; j < i; j++) free_frame(child_user_frames[j]);
            free_frame(child_dir_frame);
            free_frame(child_frame);
            return -ENOMEM;
        }
    }
    
    /* 5. Mapear código (compartido) y datos (nuevos frames) en la tabla del hijo */
    set_user_pages(child_dir);
    
    page_table_entry *parent_PT = get_PT(current());
    page_table_entry *child_PT = child_dir;
    
    /* 6. Bucle de copia de memoria del espacio de usuario usando mapeo temporal */
    for (int i = 0; i < NUM_PAG_DATA; i++) {
        int pag_log = PAG_LOG_INIT_DATA + i;
        int child_phys_frame = child_user_frames[i];
        
        /* Mapear la página en la tabla del hijo */
        set_ss_pag(child_PT, pag_log, child_phys_frame);
        
        /* Mapear temporalmente en una entrada libre del padre */
        int temp_pag = PAG_LOG_INIT_DATA + NUM_PAG_DATA + i;
        set_ss_pag(parent_PT, temp_pag, child_phys_frame);
        
        /* Copiar bloque de memoria */
        copy_data((void *)(pag_log << 12), (void *)(temp_pag << 12), PAGE_SIZE);
        
        /* Eliminar la entrada temporal */
        del_ss_pag(parent_PT, temp_pag);
    }
    
    /* Flush de la TLB para desvalidar los mapeos temporales */
    set_cr3(current()->dir_pages_baseAddr);
    
    /* 7. Asignar nuevo PID único al hijo */
    static int global_pid = 2;
    child_union->task.PID = global_pid++;
    child_union->task.dir_pages_baseAddr = child_dir;
    
    /* 8. Forjar la pila de sistema del hijo con ret_from_fork */
    child_union->stack[KERNEL_STACK_SIZE - 10] = 0; /* EAX guardado = 0 para el hijo */
    child_union->stack[KERNEL_STACK_SIZE - 18] = (unsigned long) ret_from_fork;
    child_union->stack[KERNEL_STACK_SIZE - 19] = 0; /* Fake EBP */
    
    child_union->task.kernel_esp = (unsigned long *) &(child_union->stack[KERNEL_STACK_SIZE - 19]);
    
    /* 9. Encolar al hijo en la readyqueue */
    list_add_tail(&(child_union->task.list), &readyqueue);
    
    /* 10. Retornar el PID del hijo al padre */
    return child_union->task.PID;
}

void sys_exit(void) {
    int page_dir_frame = ((unsigned long)current()->dir_pages_baseAddr) >> 12;
    page_table_entry *PT = get_PT(current());
    
    /* 1. Liberar los frames de memoria del usuario (Datos y Pila) */
    for (int i = 0; i < NUM_PAG_DATA; i++) {
        int pag_log = PAG_LOG_INIT_DATA + i;
        int frame = get_frame(PT, pag_log);
        if (frame >= 0) {
            free_frame(frame);
            del_ss_pag(PT, pag_log);
        }
    }
    
    /* 2. Liberar el directorio de páginas */
    free_frame(page_dir_frame);
    
    /* 3. Desencolar el PCB */
    update_process_state_rr(current(), NULL);
    
    /* 4. Ceder la CPU definitivamente al siguiente proceso */
    sched_next_rr();
}