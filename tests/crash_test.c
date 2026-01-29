#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

struct termios original_termios;

void enableRawMode()
{
	// Use tcgetattr() to read the current terminal settings into it
	tcgetattr(STDIN_FILENO, &original_termios);

	// Make a copy of my original settings, Modify the copy to disable Canonical mode and then apply the modified settings to the terminal
	struct termios new_termios;

	// Copy original variable to new variable
	new_termios = original_termios;

	// Clear the flag-Modify this so typed characters dont appear automatically; Disable CTRL+C so it becomes a regular input character
	new_termios.c_lflag &= ~(ICANON | ECHO | ISIG);

	// Disable input post-processing
	new_termios.c_iflag &= ~(IXON | ICRNL);

	// Apply the modified settings to the terminal
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios);

}

void disableRawMode()
{
	// restore terminal in this function
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}	

void test_crash() 
{
	 printf("=== TEST: Crash Scenario ===\n");
    	printf("This will crash. After, check if terminal works.\n");
    	printf("Press any key to trigger crash...\n");
    	fflush(stdout);
    
    	char c;
    	read(STDIN_FILENO, &c, 1);
    
    	int *ptr = NULL;
    	*ptr = 42;  // Crash!
}

int main()
{
	atexit(disableRawMode);
	enableRawMode();

	test_crash();

	return 0;
}
