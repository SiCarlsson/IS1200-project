/* mipslabfunc.c
	 This file written 2015 by F Lundevall
	 Some parts are original code written by Axel Isaksson

	 For copyright and licensing, see file COPYING */

#include <stdint.h>	 /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declatations for these labs */

/* Declare a helper function which is local to this file */
static void num32asc(char *s, int);

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

// Size of the display
const int displayW = 128;
const int displayH = 32;
const int displayPageH = 8;

// Size of bird in pixels
const int birdsize = 3;

// size of obstacle in pixels
const int obstacleW = 5;

/* quicksleep:
	 A simple function to create a small delay.
	 Very inefficient use of computing resources,
	 but very handy in some special cases. */
void quicksleep(int cyc)
{
	int i;
	for (i = cyc; i > 0; i--)
		;
}

uint8_t spi_send_recv(uint8_t data)
{
	while (!(SPI2STAT & 0x08))
		;
	SPI2BUF = data;
	while (!(SPI2STAT & 1))
		;
	return SPI2BUF;
}

void display_init(void)
{
	DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);

	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);

	spi_send_recv(0x8D);
	spi_send_recv(0x14);

	spi_send_recv(0xD9);
	spi_send_recv(0xF1);

	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);

	spi_send_recv(0xA1);
	spi_send_recv(0xC8);

	spi_send_recv(0xDA);
	spi_send_recv(0x20);

	spi_send_recv(0xAF);
}

void display_string(int line, char *s)
{
	int i;
	if (line < 0 || line >= 4)
		return;
	if (!s)
		return;

	for (i = 0; i < 16; i++)
		if (*s)
		{
			textbuffer[line][i] = *s;
			s++;
		}
		else
			textbuffer[line][i] = ' ';
}

void display_image(int x, const uint8_t *data)
{
	int i, j;

	for (i = 0; i < 4; i++)
	{
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(x & 0xF);
		spi_send_recv(0x10 | ((x >> 4) & 0xF));

		DISPLAY_CHANGE_TO_DATA_MODE;

		for (j = 0; j < 128; j++)
			spi_send_recv(~data[i * 128 + j]);
	}
}

void display_update(void)
{
	int i, j, k;
	int c;
	for (i = 0; i < 4; i++)
	{
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(0x0);
		spi_send_recv(0x10);

		DISPLAY_CHANGE_TO_DATA_MODE;

		for (j = 0; j < 16; j++)
		{
			c = textbuffer[i][j];
			if (c & 0x80)
				continue;

			for (k = 0; k < 8; k++)
				spi_send_recv(font[c * 8 + k]);
		}
	}
}

/*
	Helper function to clear all strings of text on display
*/
void display_clear()
{
	display_string(0, "");
	display_string(1, "");
	display_string(2, "");
	display_string(3, "");
}

void display_clear_pixels()
{
	int byte = 0;
	for (byte = 0; byte < 512; byte++)
	{
		displayBuffer[byte] = 255;
	}
}

/*
	Helper function to display four lines of text
*/
void display_string_array(char *stringArray[], int arraySize)
{
	display_clear();

	// Itterates stringArray and displays each instance
	int i = 0;
	while (i < arraySize)
	{
		display_string(i, stringArray[i]);
		i++;
	}
	display_update();
}

void display_pixel(int xPos, int yPos)
{
	// Failsafe, pixel cannot be outside screen
	if (xPos < 0 || xPos > 128 || yPos < 0 || yPos > 32)
		return;

	int yByte;

	if (yPos >= 24 && yPos < 32)
	{
		yByte = twoToPower(yPos - (displayPageH * 3));
		displayBuffer[xPos + (displayW * 3)] &= (~yByte);
	}
	if (yPos >= 16 && yPos < 24)
	{
		yByte = twoToPower(yPos - (displayPageH * 2));
		displayBuffer[xPos + (displayW * 2)] &= (~yByte);
	}
	if (yPos >= 8 && yPos < 16)
	{
		yByte = twoToPower(yPos - displayPageH);
		displayBuffer[xPos + displayW] &= (~yByte);
	}
	if (yPos < 8)
	{
		yByte = twoToPower(yPos);
		displayBuffer[xPos] &= (~yByte);
	}
}

// Uses display_pixel() to draw a bird
void display_bird(int xPos, int yPos)
{
	// (x-1, y+1) (x, y+1) (x+1, y+1)
	// (x-1, y)	  (x, y)   (x+1, y)
	// (x-1, y-1) (x, y-1) (x+1, y-1)

	int i = 0;
	int j = 0;
	while (i < birdsize)
	{
		while (j < birdsize)
		{
			display_pixel((xPos + i - 1), (yPos + j - 1));
			j++;
		}
		i++;
		j = 0;
	}
}

// Uses display_pixel to draw an obstacle
void display_obstacle(int xPos, int yPos, int gap)
{
	int i = 0;
	int j = 0;
	int gapCounter = 0;
	while (i < 3)
	{
		while (j <= displayH)
		{
			if (j <= yPos || j >= (yPos + gap))
				display_pixel((xPos + i - 1), j);

			j++;
		}
		i++;
		j = 0;
	}
}

/* Helper function, local to this file.
	 Converts a number to hexadecimal ASCII digits. */
static void num32asc(char *s, int n)
{
	int i;
	for (i = 28; i >= 0; i -= 4)
		*s++ = "0123456789ABCDEF"[(n >> i) & 15];
}

/*
 * itoa
 *
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 *
 * The integer argument is converted to a string
 * of digits representing the integer in decimal format.
 * The integer is considered signed, and a minus-sign
 * precedes the string of digits if the number is
 * negative.
 *
 * This routine will return a varying number of digits, from
 * one digit (for integers in the range 0 through 9) and up to
 * 10 digits and a leading minus-sign (for the largest negative
 * 32-bit integers).
 *
 * If the integer has the special value
 * 100000...0 (that's 31 zeros), the number cannot be
 * negated. We check for this, and treat this as a special case.
 * If the integer has any other value, the sign is saved separately.
 *
 * If the integer is negative, it is then converted to
 * its positive counterpart. We then use the positive
 * absolute value for conversion.
 *
 * Conversion produces the least-significant digits first,
 * which is the reverse of the order in which we wish to
 * print the digits. We therefore store all digits in a buffer,
 * in ASCII form.
 *
 * To avoid a separate step for reversing the contents of the buffer,
 * the buffer is initialized with an end-of-string marker at the
 * very end of the buffer. The digits produced by conversion are then
 * stored right-to-left in the buffer: starting with the position
 * immediately before the end-of-string marker and proceeding towards
 * the beginning of the buffer.
 *
 * For this to work, the buffer size must of course be big enough
 * to hold the decimal representation of the largest possible integer,
 * and the minus sign, and the trailing end-of-string marker.
 * The value 24 for ITOA_BUFSIZ was selected to allow conversion of
 * 64-bit quantities; however, the size of an int on your current compiler
 * may not allow this straight away.
 */
#define ITOA_BUFSIZ (24)
char *itoaconv(int num)
{
	register int i, sign;
	static char itoa_buffer[ITOA_BUFSIZ];
	static const char maxneg[] = "-2147483648";

	itoa_buffer[ITOA_BUFSIZ - 1] = 0; /* Insert the end-of-string marker. */
	sign = num;						  /* Save sign. */
	if (num < 0 && num - 1 > 0)		  /* Check for most negative integer */
	{
		for (i = 0; i < sizeof(maxneg); i += 1)
			itoa_buffer[i + 1] = maxneg[i];
		i = 0;
	}
	else
	{
		if (num < 0)
			num = -num;		 /* Make number positive. */
		i = ITOA_BUFSIZ - 2; /* Location for first ASCII digit. */
		do
		{
			itoa_buffer[i] = num % 10 + '0'; /* Insert next digit. */
			num = num / 10;					 /* Remove digit from number. */
			i -= 1;							 /* Move index to next empty position. */
		} while (num > 0);
		if (sign < 0)
		{
			itoa_buffer[i] = '-';
			i -= 1;
		}
	}
	/* Since the loop always sets the index i to the next empty position,
	 * we must add 1 in order to return a pointer to the first occupied position. */
	return (&itoa_buffer[i + 1]);
}

/*
	Helper function to display all current highscores
*/
void display_highscore()
{
	display_clear();
	display_string(0, "Highscores");

	int counter = 0;
	for (counter = 0; counter < 3; counter++)
	{
		char temp[16];
		char dots[] = "..........";
		char *name = scoreboardNames[counter];
		int score = scoreboard[counter];
		char *scoreStr = itoaconv(score);

		int i = 0;
		// Inserts name to temp
		while (name[i] != '\0')
		{
			temp[i] = name[i];
			i++;
		}

		// Inserts dots in between
		int j = 0;
		while (dots[j] != '\0')
		{
			temp[i] = dots[j];
			i++;
			j++;
		}

		// Inserts score to temp
		// Find length of score (if less than 3, add zeros in front)
		int scoreLength = 0;
		while (scoreStr[scoreLength] != '\0')
			scoreLength++;

		// Add leading zeros based on score length
		j = 0;
		if (scoreLength == 1)
		{
			temp[i++] = '0';
			temp[i++] = '0';
		}

		else if (scoreLength == 2)
			temp[i++] = '0';

		// Copy scoreStr to temp
		while (scoreStr[j] != '\0')
			temp[i++] = scoreStr[j++];

		display_string(counter + 1, temp);
	}
	display_update();
}