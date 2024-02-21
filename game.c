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
void display_flash_gameover();
void main_menu();

// Constants
const int jumpSpeed = 3;
const int moveSpeed = 2;
const int counterLimit = 2;

struct Bird
{
  int posX;
  int posY;
  int speedX; // Left/right speed
  int speedY; // Gravity
};

struct Obstacle
{
  int posX;
  int posY;
  int speedX;
  int gap;
};

void game_loop(void)
{

  // Initialize bird
  struct Bird bird;
  bird.posX = 10;
  bird.posY = 0;
  bird.speedX = 0;
  bird.speedY = 3;

  // Initialize obstacles
  struct Obstacle obstacles[3];

  // Variable keeps track of an active game, 0 if game is over
  int activeGame = 1;

  // initialisera Timer 2 (Från labb3)
  TMR2 = 0x0;
  T2CON = 0x70;
  int timeoutcount = 0;

  // 80 MHz = 80'000'000, Clock rate divider, Time out period
  // Beräkning för att få ett värde in i 16-bitars format
  PR2 = ((80000000 / 256) / 50);
  T2CONSET = 0x8000;

  int counter2 = 0;
  int counter3 = 0;
  int counter4 = 0;

  while (activeGame)
  {
    // usage of the timer from lab 3
    if (IFS(0) & 0x100)
    {
      timeoutcount++;
      IFSCLR(0) = 0x100;
    }

    if (timeoutcount == 1)
    {
      display_clear_pixels();

      display_bird(bird.posX, bird.posY); // Bird is written to the buffer

      /*
        Takes user inputs
      */
      if (btn2pressed())
        counter2++;
      if (btn3pressed())
        counter3++;
      if (btn4pressed())
        counter4++;

      /*
        Change birds coordinates
      */
      // Controls when the moves right
      if (counter2 >= counterLimit)
      {
        bird.speedX = moveSpeed;
        counter2 = 0;
      }
      // Controls when the bird jumps
      if (counter3 >= counterLimit)
      {
        bird.speedY -= jumpSpeed;
        counter3 = 0;
      }
      // Controls when the bird moves left
      if (counter4 >= counterLimit)
      {
        bird.speedX = (moveSpeed) * (-1);
        counter4 = 0;
      }

      // Store values in bird
      bird.posY += bird.speedY;
      bird.posX += bird.speedX;

      /*
        Keeps the bird on the screen
      */
      if (bird.posY <= 1) // Keeps the bird from jumping out of screen (top)
        bird.posY = 1;
      if (bird.posX <= 1) // Keeps the bird from jumping out of screen (left)
        bird.posX = 1;
      if (bird.posX >= 126) // Keeps the bird from jumping out of screen (right)
        bird.posX = 126;

      /*
        Makes the speed go back to its original value
      */
      if (bird.speedY < 1)
        bird.speedY += 1; // Slow traceback
      if (bird.speedX != 0)
        bird.speedX = 0; // Direct traceback

      /*
        Game Over Controllers
      */
      // If bird touches the ground the game is over
      if (bird.posY >= 31)
      {
        activeGame = 0;
        break;
      }
      // ADD OBSTACLE CHECK

      /*
        Final updates
      */
      // Sending the display buffer to OLED screen
      display_image(0, displayBuffer);

      // Reset timeoutcounter
      timeoutcount = 0;
    }
  }
  game_over();
}

void game_over()
{
  // Loop forces all buttons to be disabled temporarily
  while (1)
  {
    display_clear_pixels();
    display_clear();

    display_flash_gameover();
    break;
  }

  quicksleep(5000000);
  display_string(3, " Continue: btn3");
  display_update();

  // TODO
  // GET NAME
  // DISPLAY NAME AND SCORE

  // Loops waits for user input before going to the main menu
  while (1)
  {
    if (btn3pressed())
    {
      quicksleep(1000000);
      break;
    }
  }
  main_menu();
}
