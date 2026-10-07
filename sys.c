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

struct write_params {
	int fd;
	char *buffer;
	int size;
};

int sys_write(void *params)
{
	struct write_params p;

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

	char s_buff[p.size];
	copy_from_user(p.buffer, s_buff, p.size);
	return sys_write_console(s_buff, p.size);
}

int sys_gettime(void)
{
	return zeos_ticks;
}
