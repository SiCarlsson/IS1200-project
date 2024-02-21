// game.c
// This file written 2024 by S Carlsson and E Lindblom

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include <stdlib.h>  /* Declarations to use Random generator */
#include "mipslab.h" /* Needed for displayBuffer */

/*
  This file handles all logic of the actual game
*/

// Declaration of functions
void game_over();

struct Bird
{
  int posX;
  int posY;
  double speedX;
  double speedY;
};

struct Obstacle
{
  double posX;
  double posY;
  double speedX;
  int gap;
};

void game_loop(void)
{
  struct Bird bird;
  bird.posX = 10;
  bird.posY = 16;
  bird.speedY = 1;

  struct Obstacle obstacles[3];

  while (1)
  {
    display_clear_pixels();

    // Bird is written to the buffer
    display_bird(bird.posX, bird.posY);

    bird.posY += bird.speedY;

    /* GAME OVER Controllers */
    if (bird.posY >= 31)
      game_over();
    // ADD OBSTACLE CHECK

    // Sending the display buffer to OLED screen
    display_image(0, displayBuffer);

    quicksleep(1000000);

    if (btn4pressed())
    {
      quicksleep(1000000);
      break;
    }
  }
}

void game_over()
{
  display_clear_pixels();
  display_clear();

  display_string(1, "GAME OVER");
  display_update();

  while (1)
  {
    if (btn4pressed())
    {
      quicksleep(1000000);
      main_menu();
    }
  }
}
