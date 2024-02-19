// menufuncs.c
// This file written 2024 by S Carlsson and E Lindblom

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */

// Declaration of functions
int getbtns(void);
void display_clear();
void instructions();

/*
  MENU FUNCTIONS
*/
// Main menu
void main_menu()
{
  display_clear();

  // Gets input 4 to 2
  int choice = 0;
  int button = getbtns();
  
  // Displays main menu
  char *menu[] = {"Main menu", "1. Instructions", "2. Play game", "3. High scores"};
  display_string_array(menu, 4);


  // Converts input to int -> to use with switch-case
  if (button & 4)
    choice = 1;
  else if (button & 2)
    choice = 2;
  else if (button & 1)
    choice = 3;

  int timer = 1000;
  switch (choice)
  {
    display_clear();

  // 1. Instructions
  case 1:
    instructions();
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

void instructions()
{
  char *instructions[] = {"Instructions", "BTN4: Jump", "BTN3: Go left", "BTN2: Go right"};
  display_string_array(instructions, 4);

  int button;

  // Keeps the user on the instructions page untill BTN4 is pressed
  while (1)
  {
    button = getbtns();
    if (button & 2)
    {
      break;
    }
  }
}