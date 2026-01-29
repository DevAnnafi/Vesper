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



	return 0;

}
