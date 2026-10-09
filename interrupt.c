/*
 * interrupt.c -
 */
#include <types.h>
#include <interrupt.h>
#include <segment.h>
#include <hardware.h>
#include <io.h>
#include <utils.h> /* for hex_to_str */

#include <zeos_interrupt.h>

Gate idt[IDT_ENTRIES];
Register    idtR;

char char_map[] =
{
  '\0','\0','1','2','3','4','5','6',
  '7','8','9','0','\'','�','\0','\0',
  'q','w','e','r','t','y','u','i',
  'o','p','`','+','\0','\0','a','s',
  'd','f','g','h','j','k','l','�',
  '\0','�','\0','�','z','x','c','v',
  'b','n','m',',','.','-','\0','*',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','7',
  '8','9','-','4','5','6','+','1',
  '2','3','0','\0','\0','\0','<','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0'
};

/*
 * handlers below defined in entry.S
 */

/* exceptions */
extern void pagefault_handler(void);

/* interrupts */
extern void timer_handler(void);
extern void keyboard_handler(void);

/* syscalls */
void system_call_handler(void);

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE INTERRUPTION GATE FLAGS:                          R1: pg. 5-11  */
  /* ***************************                                         */
  /* flags = x xx 0x110 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE TRAP GATE FLAGS:                                  R1: pg. 5-11  */
  /* ********************                                                */
  /* flags = x xx 0x111 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);

  //flags |= 0x8F00;    /* P = 1, D = 1, Type = 1111 (Trap Gate) */
  /* Changed to 0x8e00 to convert it to an 'interrupt gate' and so
     the system calls will be thread-safe. */
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}


void setIdt()
{
  /* Program interrups/exception service routines */
  idtR.base  = (DWord)idt;
  idtR.limit = IDT_ENTRIES * sizeof(Gate) - 1;
  
  set_handlers();

  /* ADD INITIALIZATION CODE FOR INTERRUPT VECTOR */
  setInterruptHandler(14, pagefault_handler, 0);

  setInterruptHandler(32, timer_handler, 0);
  setInterruptHandler(33, keyboard_handler, 0);

  /* syscalls */
  setTrapHandler(0x93, system_call_handler, 3);

  set_idt_reg(&idtR);
}

struct sys_stack {
  /* SAVE_ALL */
  unsigned int edx;
  unsigned int ecx;
  unsigned int ebx;
  unsigned int esi;
  unsigned int edi;
  unsigned int ebp;
  unsigned int eax;
  unsigned int ds;
  unsigned int es;
  unsigned int fs;
  unsigned int gs;
  /* params */
  unsigned int error_code;
  unsigned int eip;
  unsigned int cs;
  unsigned int eflags;
  unsigned int esp;
  unsigned int ss;
};

static void print_reg(char *name, unsigned int val) {
  char buf[9];
  printk(name);
  printk(": 0x");
  hex_to_str(val, buf);
  printk(buf);
  printk("\n");
}

void pagefault_routine(struct sys_stack *ctx, unsigned int cr2) {
  printk("\nProcess generated a PAGE FAULT exception\n");

  print_reg("EIP (offending instr)", ctx->eip);
  print_reg("CR2 (offending addr) ", cr2);
  print_reg("error code", ctx->error_code);

  print_reg("EAX", ctx->eax);
  print_reg("EBX", ctx->ebx);
  print_reg("ECX", ctx->ecx);
  print_reg("EDX", ctx->edx);
  print_reg("ESI", ctx->esi);
  print_reg("EDI", ctx->edi);
  print_reg("EBP", ctx->ebp);
  print_reg("ESP", ctx->esp);
  print_reg("EFLAGS", ctx->eflags);
  print_reg("CS", ctx->cs);
  print_reg("DS", ctx->ds);
  print_reg("ES", ctx->es);
  print_reg("FS", ctx->fs);
  print_reg("GS", ctx->gs);
  print_reg("SS", ctx->ss);

  while (1);
}

void timer_routine(void) {
  zeos_show_clock();
  zeos_ticks++;
}

void keyboard_routine(void) {
  unsigned char ch = inb(0x60);

  if (!(ch & 0x80)) {
    printc_xy(75, 5, char_map[ch & 0x7f]);
  }
}
