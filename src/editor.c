#include <stdio.h>
#include <unistd.h>
#include "terminal.h"

void editorLoop()
{
	while (1)
	{
		char c;
		read(STDIN_FILENO, &c, 1);
	}
}
