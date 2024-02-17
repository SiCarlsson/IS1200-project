/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog

   For copyright and licensing, see file COPYING */

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declatations for these labs */

volatile int *trisE;
volatile int *portE;

int mytime = 0x5957;

char textstring[] = "text, more text, and even more text!";

/* Interrupt Service Routine */
void user_isr(void)
{
  return;
}

/* Lab-specific initialization goes here */
void labinit(void)
{
  // skapar en pointer som pekar på adressen
  trisE = (volatile int *)0xbf886100;
  *trisE = ~(0xff);

  portE = (volatile int *)0xbf886110;
  *portE = PORTE & ~(0xff);

  TRISDSET = (0xfe0);
}

/* This function is called repetitively from the main program */
void labwork(void)
{
  int buttons = getbtns();
  int switches = getsw();

  // kollar om knapp 4 är nedtryckt
  if (buttons & 4)
  {
    mytime = mytime & 0x0fff;
    mytime = (switches << 12) | mytime;
  }

  // kollar om knapp 3 är nedtryckt
  if (buttons & 2)
  {
    mytime = mytime & 0xf0ff;
    mytime = (switches << 8) | mytime;
  }

  // kollar om knapp 1 är nedtryckt
  if (buttons & 1)
  {
    mytime = mytime & 0xff0f;
    mytime = (switches << 4) | mytime;
  }

  delay(1000);
  time2string(textstring, mytime);
  display_string(3, textstring);
  display_update();
  tick(&mytime);
  display_image(96, icon);
}
