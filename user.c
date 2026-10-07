#include <libc.h>

char buff[24];

int pid;

int result;

void hex_to_str(unsigned int val, char *buf)
{
  const char digits[] = "0123456789ABCDEF";
  int i;
  for (i = 7; i >= 0; i--) {
    buf[i] = digits[val & 0xF];
    val >>= 4;
  }
  buf[8] = '\0';
}


int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

	const char *msg = "Welcome to ZeOS\n";
	write(1, msg, strlen(msg));

	while(1) {
		int ticks = gettime();
		char buf[8];
		hex_to_str(ticks, buf);
		write(1, buf, strlen(buf));
		write(1, "\n", 1);
	}
}
