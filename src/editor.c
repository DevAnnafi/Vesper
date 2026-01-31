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
		printf("\x1b[2J");
		printf("\x1b[H");

		printf("Vesper Rocks!\n");

		printf("\x1b[%d;%dH", state.cursor_y + 1, state.cursor_x + 1);

		fflush(stdout);

		char c;
		read(STDIN_FILENO, &c, 1);
    
		if (c == 'q') 
		{
			break;
		}

		else if (c == 'h') 
		{
			if (state.cursor_x > 0) 
			{ 
				state.cursor_x--;
			}
		}

		else if (c == 'l') 
		{
			if (state.cursor_x < state.screen_cols - 1) 
			{
				state.cursor_x++;
			}
		}

		else if (c == 'k') 
		{
			if(state.cursor_y > 0) 
			{
				state.cursor_y--;
			}
		}

		else if (c == 'j') 
		{
			if(state.cursor_y < state.screen_rows - 1) 
			{
				state.cursor_y++;

			}
		}
		
		/* Print AFTER updating state
		printf("Key: %c | Cursor: (%d, %d)\r\n", c, state.cursor_x, state.cursor_y);
		fflush(stdout);
	*/
	} 

}
