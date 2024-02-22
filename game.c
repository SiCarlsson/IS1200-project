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
const int counterLimit = 2; // Works as a delay-function to button inputs

int obstacleAmount = 2;
int obstacleGap = 27;
int obstacleSpacing = 64; // Display width / 2

struct Bird
{
  int posX;
  int posY;
  int speedX; // Left/right speed
  int speedY; // Gravity
  int score;
};

struct Obstacle
{
  int posX;
  int posY;
  int speedX;
  int pointGiven;
};

void game_loop(void)
{
  /*
    Initialization of game components
  */
  // Initialize bird
  struct Bird bird;
  bird.posX = 15;
  bird.posY = 0;
  bird.speedX = 0;
  bird.speedY = 3;
  bird.score = 0;

  // Initialize obstacles
  struct Obstacle obstacles[obstacleAmount];
  int obstacleStartX = 127;

  // Sets the same values for all three obstacles
  int i;
  for (i = 0; i < obstacleAmount; i++)
  {
    obstacles[i].posX = obstacleStartX;
    obstacles[i].posY = 2;
    obstacles[i].speedX = -3;
    obstacles[i].pointGiven = 0;

    // Spread the obstacles
    obstacleStartX += obstacleSpacing;
  }

  // Variable keeps track of an active game, 0 if game is over
  int activeGame = 1;

  // initialisera Timer 2 (Från labb3)
  TMR2 = 0x0;
  T2CON = 0x70;
  int timeoutcount = 0;

  // 80 MHz = 80'000'000, Clock rate divider, Time out period
  // Beräkning för att få ett värde in i 16-bitars format
  PR2 = ((80000000 / 256) / 25);
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

      /*
        Displays bird and obstacle
      */
      display_bird(bird.posX, bird.posY); // Bird is written to the buffer

      int i; // Used for all loops in the game
      for (i = 0; i < obstacleAmount; i++)
      {
        if (obstacles[i].posX < 128)
          display_obstacle(obstacles[i].posX, obstacles[i].posY, obstacleGap);
      }

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

      bird.posY += bird.speedY;
      bird.posX += bird.speedX;

      /*
        Change object coordinates
      */
      // Move all obstacles to the left
      for (i = 0; i < obstacleAmount; i++)
        obstacles[i].posX += obstacles[i].speedX;

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
        Checks if score should be added to bird
      */
      for (i = 0; i < obstacleAmount; i++)
      {
        if (bird.posX > (obstacles[i].posX - 1)) // If the bird has passed the obstacle
          bird.score++;
      }

      /*
        Resets the obstacles
      */
      for (i = 0; i < obstacleAmount; i++)
      {
        if (obstacles[i].posX <= 1)
          obstacles[i].posX = 128;
      }

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

  quicksleep(6000000);
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
