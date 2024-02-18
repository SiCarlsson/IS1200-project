// projectfunc.c
// This file written 2024 by S Carlsson and E Lindblom

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */

int getbtns(void)
{
  return ((PORTD & 0xe0) >> 5);
}

void main_menu()
{
  // Displays main menu
  display_string(0, "Main menu");
  display_string(1, "1. Instructions");
  display_string(2, "2. Play game");
  display_string(3, "3. High scores");
  display_update();

  // Gets input 4 to 2
  int choice = 0;
  int button = getbtns();

  // Converts input to int -> to use with switch-case
  if (button & 4)
  {
    choice = 1;
  }
  else if (button & 2)
  {
    choice = 2;
  }
  else if (button & 1)
  {
    choice = 3;
  }

  int timer = 1000;
  switch (choice)
  {
    display_string(0, "");
    display_string(1, "");
    display_string(2, "");
    display_string(3, "");

  // 1. Instructions
  case 1:
    while (timer != 0)
    {
      display_string(1, "Instructions");
      display_update();
      timer--;
    }
    break;

  // 2. Play game
  case 2:
    while (timer != 0)
    {

      display_string(2, "Play game");
      display_update();
      timer--;
    }
    break;

  // 3. High scores
  case 3:
    while (timer != 0)
    {

      display_string(3, "High scores");
      display_update();
      timer--;
    }
    break;

  default:
    break;
  }
}