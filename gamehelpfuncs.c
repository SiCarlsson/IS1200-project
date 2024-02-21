// gamehelpfuncs.c
// This file written 2024 by S Carlsson and E Lindblom

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */

void display_flash_gameover()
{
  int timeoutcount = 0;
  int flashes = 0;

  // initialisera Timer 2 (Från labb3)
  TMR2 = 0x0;
  T2CON = 0x70;

  // 80 MHz = 80'000'000, Clock rate divider, Time out period
  // Beräkning för att få ett värde in i 16-bitars format
  PR2 = ((80000000 / 256) / 10);
  T2CONSET = 0x8000;

  display_string(1, "   GAME OVER");
  display_update();

  while (flashes < 8)
  {
    // usage of the timer from lab 3
    if (IFS(0) & 0x100)
    {
      timeoutcount++;
      IFSCLR(0) = 0x100;
    }

    if (timeoutcount == 5)
    {
      if (flashes % 2 == 0)
        display_string(1, "");
      else
        display_string(1, "   GAME OVER");

      display_update();

      flashes++;
      timeoutcount = 0;
    }
  }

  display_string(1, "   GAME OVER");
  display_update();
}