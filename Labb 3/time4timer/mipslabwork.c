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

// deklarerar port e och trise e
volatile int *trisE;
volatile int *portE;

int timeoutcount = 0;

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
  *trisE = 0x00;

  portE = (volatile int *)0xbf886110;
  *portE = PORTE & 0x00;

  // TRISD hanterar inputs
  // Bit 11-8 för spakar, bitarna 7-5 för knappar
  TRISDSET = 0x0fe0; // OR behåller bitarna 11-5 bits

  // initialisera Timer 2
  TMR2 = 0x0;
  T2CON = 0x70;

  // ska vara timeouts på 100 ms (förutsätter 80 MHz)
  // 80 MHz = 80'000'000, Clock rate divider, Time out period
  // Beräkning för att få ett värde in i 16-bitars format
  // Delat på 10 för 10 timercounts
  PR2 = ((80000000 / 256) / 10);
  T2CONSET = 0x8000;

  return;
}

void labwork(void)
{
  // Hämtar inputs
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

  // AND med 0001 0000 0000, om den 8onde biten är en 1a, inkrementera countern
  // 0x100 för att vi delade tidigare på 256
  if (IFS(0) & 0x100)
  {
    timeoutcount++;

    // Återställ flaggan, maskar ut de som ska nollställas
    IFSCLR(0) = 0x100;
  }

  // Kontrollerar countern
  if (timeoutcount == 10)
  {
    time2string(textstring, mytime);
    display_string(3, textstring);
    display_update();
    tick(&mytime);
    display_image(96, icon);

    // Återställ counter
    timeoutcount = 0;
  }
}
