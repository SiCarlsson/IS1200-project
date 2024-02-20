// game.c
// This file written 2024 by S Carlsson and E Lindblom

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */

/*
  This file handles all logic of the actual game
*/

// Size of bird in pixels
const int birdsize = 3;

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
