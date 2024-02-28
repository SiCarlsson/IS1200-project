// io.c
// This file written 2024 by S Carlsson and E Lindblom

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */

/*
  Functions to get the inputs of buttons and switches
*/
int getbtns(void)
{
  return ((PORTD & 0xe0) >> 5);
}

int getsw(void)
{
  return (PORTD & 0x0f00) >> 8;
}

/*
  Helper functions to check if a button is pressed
*/
int btn4pressed(void)
{
  return (getbtns() & 0x4) >> 2;
}

int btn3pressed(void)
{
  return (getbtns() & 0x2) >> 1;
}

int btn2pressed(void)
{
  return (getbtns() & 0x1);
}