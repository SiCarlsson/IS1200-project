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
void highscores();
int customRandom(int gap);
char *itoaconv(int num);
int reachedHighscore(int currentScore, int highscores[]);
void *getInitials();
void updateHighscore(int pos, int score, char *name);

// Constants
const int jumpSpeed = 3;
const int moveSpeed = 2;
const int counterLimit = 2; // Works as a delay-function to button inputs

int obstacleAmount = 4;
int obstacleSpacing = 32; // Display width / 4

// HIGHSCORES
int scoreboard[] = {0, 0, 0};

char scoreboardName1[] = "NaN";
char scoreboardName2[] = "NaN";
char scoreboardName3[] = "NaN";

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
  int gap;
  int visible;
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
  int obstacleStartX = 128;

  // Sets the same values for all three obstacles
  int i;
  for (i = 0; i < obstacleAmount; i++)
  {
    obstacles[i].posX = obstacleStartX;
    obstacles[i].posY = 2;
    obstacles[i].speedX = -3;
    obstacles[i].pointGiven = 0;
    obstacles[i].gap = 27;
    // Spread the obstacles
    obstacleStartX += obstacleSpacing;
  }

  obstacles[0].visible = 1;
  obstacles[1].visible = 0;
  obstacles[2].visible = 1;
  obstacles[3].visible = 0;

  // Variable keeps track of an active game, 0 if game is over
  int activeGame = 1;

  // Initialize score counter
  PORTE = 0x0;

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
        if (obstacles[i].posX < 128 && obstacles[i].visible == 1)
          display_obstacle(obstacles[i].posX, obstacles[i].posY, obstacles[i].gap);
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
        if (bird.posX > obstacles[i].posX) // If the bird has passed the obstacle
        {
          if (obstacles[i].pointGiven == 0 && obstacles[i].visible == 1)
          {
            obstacles[i].pointGiven = 1;
            bird.score += 1;
            PORTE += 1;
          }
        }
      }

      /*
        Resets the obstacles
      */
      int resetCounter = 0;
      for (i = 0; i < obstacleAmount; i++)
      {
        if (obstacles[i].posX <= 1)
        {
          // Updates on all resets
          obstacles[i].posX = 128;
          obstacles[i].posY = customRandom(obstacles[i].gap);
          obstacles[i].pointGiven = 0;

          // Updates every 4th reset
          if (resetCounter % 4 == 0)
            obstacles[i].gap -= 1;

          resetCounter++;
        }
      }

      /*
        Adds more obstacles during the game
      */
      if (PORTE == 10)
        obstacles[1].visible = 1;
      if (PORTE == 28)
        obstacles[3].visible = 1;

      /*
        Game Over Controllers
      */
      // If bird touches the ground the game is over
      if (bird.posY >= 31)
      {
        activeGame = 0;
        break;
      }

      // Collission checker with obstacles
      for (i = 0; i < obstacleAmount; i++) // Obstacle width
      {
        // Can only hit visible obstacles
        if (obstacles[i].visible == 1)
        {
          if (bird.posX >= obstacles[i].posX - 1 && bird.posX <= obstacles[i].posX + 1) // x-value matches with an obstacle
          {
            // Upper obstacle hit
            if (bird.posY <= obstacles[i].posY + 1) // y-value matches with the upper obstacle
            {
              activeGame = 0;
              break;
            }

            // Lower obstacle hit
            if (bird.posY >= (obstacles[i].posY + obstacles[i].gap))
            {
              activeGame = 0;
              break;
            }
          }
        }
      }

      /*
        Makes the speed go back to its original value
      */
      if (bird.speedY < 1)
        bird.speedY += 1; // Slow traceback
      if (bird.speedX != 0)
        bird.speedX = 0; // Direct traceback

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
  int highscore = 0;
  // Loop forces all buttons to be disabled temporarily
  while (1)
  {
    display_clear_pixels();
    display_clear();

    display_flash_gameover();

    int highscorePos = reachedHighscore(PORTE, scoreboard);
    if (highscorePos != 4)
    {
      // char *initials = getInitials();
      updateHighscore(highscorePos, PORTE, getInitials());

      highscore = 1;
      break;
    }

    quicksleep(6000000);
    display_string(3, "Continue: btn3");
    display_update();

    // Loops waits for user input before going to the main menu
    while (1)
    {
      if (btn3pressed())
      {
        quicksleep(1000000);
        break;
      }
    }

    break;
  }

  PORTE = 0x0;

  if (highscore)
    highscores();
  else
    main_menu();
}
