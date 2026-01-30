#include <stdio.h>
#include <unistd.h>
#include "terminal.h"
#include "editor.h"

EditorState state;

void editorLoop()
{
	// Initialize state
	state.cursor_x = 0;
	state.cursor_y = 0;
	state.screen_rows = 24;
	state.screen_cols = 80;

	while (1)
	{
		char c;
		read(STDIN_FILENO, &c, 1);

		printf("You pressed: %c (ASCII %d)\r\n", c, c);
		fflush(stdout);

		if (c == 'q') 
		{
			break;
		}
	}
}
