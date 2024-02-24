// menufuncs.c
// This file written 2024 by S Carlsson and E Lindblom

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */

// Declaration of functions
int getbtns(void);
void display_clear();
void instructions();
void toGame();
void highscores();

/*
  MENU FUNCTIONS
*/
// Main menu
void main_menu()
{
  display_clear();

  // Gets input 4 to 2
  int choice = 0;

  // Displays main menu
  char *menu[] = {"Main menu", "1. Instructions", "2. Play game", "3. High scores"};
  display_string_array(menu, 4);

  // Converts input to int -> to use with switch-case
  if (btn4pressed())
  {
    choice = 1;
  }
  else if (btn3pressed())
  {
    choice = 2;
  }
  else if (btn2pressed())
  {
    choice = 3;
  }

  switch (choice)
  {
    display_clear();

  // 1. Instructions
  case 1:
    instructions();
    break;

  // 2. Play game
  case 2:
    toGame();
    break;

  // 3. High scores
  case 3:
    highscores();
    break;

  default:
    break;
  }
}

void instructions()
{
  char *instructions1[] = {"Manual, p.1", "BTN4: Prev. page", "BTN3: Main menu", "BTN2: Next page"};
  char *instructions2[] = {"Manual, p.2", "BTN4: Go left", "BTN3: Jump", "BTN2: Go right"};
  char *instructions3[] = {"Manual, p.3", "Each obstacle", "equals one", "point"};
  char *instructions4[] = {"Manual, p.4", "Game over", "when you hit", "an obstacle"};
  char *instructions5[] = {"Manual, p.5", "That's it!", "Go back to", "the main menu"};

  // Keeps track of the page
  int page = 1;

  // Keeps the user on the instructions page untill BTN4 is pressed
  while (1)
  {
    if (page == 1)
      display_string_array(instructions1, 4);

    else if (page == 2)
      display_string_array(instructions2, 4);

    else if (page == 3)
      display_string_array(instructions3, 4);

    else if (page == 4)
      display_string_array(instructions4, 4);

    else if (page == 5)
      display_string_array(instructions5, 4);

    // Goes to prev page if not already on page 1
    if (btn4pressed())
      if (page != 1)
        page = (page - 1);

    // Break the loop (back to main menu)
    if (btn3pressed())
    {
      quicksleep(1000000);
      break;
    }

    if (btn2pressed())
      if (page != 5)
        page = (page + 1);

    // TODO: Maybe do this in a cleaner way
    quicksleep(500000);
  }
}

void toGame()
{
  display_clear();
  display_clear_pixels();
  game_loop();
}

void highscores()
{
  char temp[] = ".............";

  char name[] = "ABC";

  int i = 0;
  int j = 0;
  while (name[i] != '\0')
  {
    temp[j] = name[i];
    i++;
    j++;
  }

  // Display the formatted string
  display_string(0, "Highscores");
  display_string(1, temp);
  display_update();

  // Keeps the user on the instructions page until BTN4 is pressed
  while (1)
  {
    if (btn3pressed())
    {
      quicksleep(1000000);
      break;
    }

    // TODO: Maybe do this in a cleaner way
    quicksleep(500000);
  }
}