#include <termios.h>
#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h>

int main() 
{
	// The struct termios variable is used to hold the original settings
	struct termios original_termios;
	
	// Use tcgetattr() to read the current terminal settings into it
	tcgetattr(STDIN_FILENO, &original_termios);

	// Make a copy of my original settings, Modify the copy to disable Canonical mode and then apply the modified settings to the terminal
	struct termios new_termios;

	// Copy original variable to new variable
	new_termios = original_termios;

	// Clear the flag 
	new_termios.c_lflag &= ~ICANON;

	// Apply the modified settings to the terminal
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios);

	// Test with one character
	printf("Type a character: ");
	char c;
	read(STDIN_FILENO, &c, 1);
	printf("\nYou typed: %c\n", c);


	return 0;

}
