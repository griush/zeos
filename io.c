/*
 * io.c - 
 */

#include <io.h>

#include <types.h>

#include <hardware.h>

/**************/
/** Screen  ***/
/**************/

#define NUM_COLUMNS 80
#define NUM_ROWS    25

#define SCREEN_BASE  0xb8000
#define BLANK_CELL   0x0200   /* space */

Byte x, y=19;

static void scroll_up(void)
{
  Word *screen = (Word *)SCREEN_BASE;
  int i;

  for (i = 0; i < (NUM_ROWS - 1) * NUM_COLUMNS; i++)
    screen[i] = screen[i + NUM_COLUMNS];

  for (i = (NUM_ROWS - 1) * NUM_COLUMNS; i < NUM_ROWS * NUM_COLUMNS; i++)
    screen[i] = BLANK_CELL;
}

static void newline(void)
{
  x = 0;
  if (y >= NUM_ROWS - 1)
    scroll_up();
  else
    y++;
}

void printc(char c)
{
  bochs_out(c);
  if (c == '\n')
  {
    newline();
  }
  else
  {
    Word ch = (Word) (c & 0x00FF) | 0x0200;
    Word *screen = (Word *)SCREEN_BASE;
    screen[(y * NUM_COLUMNS + x)] = ch;
    if (++x >= NUM_COLUMNS)
      newline();
  }
}

void printc_xy(Byte mx, Byte my, char c)
{
  Word *screen = (Word *)SCREEN_BASE;

  if (mx >= NUM_COLUMNS || my >= NUM_ROWS || c == '\n')
    return;

  bochs_out(c);
  screen[my * NUM_COLUMNS + mx] = (Word) (c & 0x00FF) | 0x0200;
}

void printk(char *string)
{
  int i;
  for (i = 0; string[i]; i++)
    printc(string[i]);
}
