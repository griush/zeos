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

int sys_write(int fd, char *buffer, int size)
{
	int cfd = check_fd(fd, ESCRIPTURA);
	if (cfd != 0)
		return cfd;

	if (buffer == 0)
		return -EFAULT; /* bad buffer */
	if (size < 0)
		return -EINVAL; /* bad write size */

	char s_buff[size];
	copy_from_user(buffer, s_buff, size);
	return sys_write_console(s_buff, size);
}

int sys_gettime(void)
{
	return zeos_ticks;
}
