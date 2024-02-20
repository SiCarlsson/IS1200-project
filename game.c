// game.c
// This file written 2024 by S Carlsson and E Lindblom

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */

/*
  This file handles all logic of the actual game
*/

struct Bird
{
  double posX;
  double posY;
  double speedX;
  double speedY;
};

struct Obstacle
{
  double posX;
  double posY;
  double speedX;
  int upsideDown;
  int spacing;
};

void game_loop(void)
{
  // FIXME: Should be while(until colission with obstacle)
  while (1)
  {

  }
}
