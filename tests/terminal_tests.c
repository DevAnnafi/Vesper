#include <termios.h>
#include <unistd.h>
#include <stdio.h>
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

// Test to see if echo is disabled
void test_echo_disabled()
{
        printf("Instructions: Type a character. You should NOT see it appear automatically.\n");
        printf("Type now: ");
	fflush(stdout);
 
        char c;
        read(STDIN_FILENO, &c, 1);
 
        printf("\nThe character you typed was: '%c' (ASCII %d)\n", c    , c);
        printf("Did it echo before this line? If NO, test PASSED!\n\n");
 
          }

int main() 
{
        atexit(disableRawMode);
	enableRawMode();

	// Test to see if CTRL-C and CTRL-S is read as an input character
	printf("Type a character: ");
	fflush(stdout);

	char c;
	read(STDIN_FILENO, &c, 1);

	if (c == 19) {
		printf("\nYou typed: CTRL-S(ASCII %d)\n", c);
	}
	else if (c == 3) {
    		printf("\nYou typed: Ctrl-C (ASCII %d)\n", c);
	} else {
    		printf("\nYou typed: %c\n", c);
	}


	test_echo_disabled();


	// Test 3: Abnormal exit (this stops execution)
    	printf("\n=== TEST: Abnormal Exit ===\n");
    	printf("Now testing abnormal exit...\n");
    	printf("Calling exit(0) without reaching return statement\n");
    	exit(0);

	return 0;

}
