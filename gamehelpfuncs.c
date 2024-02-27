// gamehelpfuncs.c
// This file written 2024 by S Carlsson and E Lindblom

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <stdlib.h>  /* Declarations to use Random generator */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h"

void display_flash_gameover()
{
  int timeoutcount = 0;
  int flashes = 0;

  // initialisera Timer 2 (Från labb3)
  TMR2 = 0x0;
  T2CON = 0x70;

  // 80 MHz = 80'000'000, Clock rate divider, Time out period
  // Beräkning för att få ett värde in i 16-bitars format
  PR2 = ((80000000 / 256) / 10);
  T2CONSET = 0x8000;

  display_string(1, "   GAME OVER");
  display_update();

  while (flashes < 8)
  {
    // usage of the timer from lab 3
    if (IFS(0) & 0x100)
    {
      timeoutcount++;
      IFSCLR(0) = 0x100;
    }

    if (timeoutcount == 5)
    {
      if (flashes % 2 == 0)
        display_string(1, "");
      else
        display_string(1, "   GAME OVER");

      display_update();

      flashes++;
      timeoutcount = 0;
    }
  }

  display_string(1, "   GAME OVER");
  display_update();
}

// Pseudo-random number generator
static uint32_t seedCounter = 1;

int customRandom(int gap)
{
  seedCounter = (seedCounter * 1664525U + 1013904223U) % UINT32_MAX;

  // Generate a random value between 0 and 15
  int posY = (seedCounter % 32) / 2;

  if (posY < 2)
    posY = 2;

  // Corrects any
  while (posY + gap > 29)
    posY -= 1;

  return posY;
}

int reachedHighscore(int currentScore, int highscores[])
{
  int boardPosition = 4;

  int i;
  for (i = 0; i < 3; i++)
  {
    if (currentScore > highscores[i])
      boardPosition--;

    else if (currentScore == highscores[i])
      return boardPosition;
  }

  return boardPosition;
}

char *getInitials()
{
  display_clear();

  const char alphabet[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

  /*
    Logic to display current state properly
  */
  // Counters
  int alphabetCounter = 0;
  int initialsCounter = 0;

  // Shows selected letters
  char selectedLetters[] = "...";
  char showSelected[] = "         ";

  while (initialsCounter < 3)
  {
    // Shows current letter
    char currentLetter[] = "    <.....>";
    currentLetter[7] = alphabet[alphabetCounter];

    // Update selected letters
    showSelected[6] = selectedLetters[0];
    showSelected[7] = selectedLetters[1];
    showSelected[8] = selectedLetters[2];

    if (btn2pressed())
    {
      alphabetCounter++;

      if (alphabetCounter > 25)
        alphabetCounter = 0;
    }
    else if (btn4pressed())
    {
      alphabetCounter--;

      if (alphabetCounter < 0)
        alphabetCounter = 25;
    }
    else if (btn3pressed())
    {
      selectedLetters[initialsCounter] = alphabet[alphabetCounter];
      initialsCounter++;
    }

    // Displays on screen
    display_string(0, "Enter name:");
    display_string(2, currentLetter);
    display_string(3, showSelected);
    display_update();

    quicksleep(600000);
  }

  // Clears screen
  display_clear();
  display_update();

  return selectedLetters;
}

