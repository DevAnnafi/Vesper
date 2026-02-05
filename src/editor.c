#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "terminal.h"
#include "editor.h"
#include "render.h"
#include "buffer.h"

EditorState state;

void sigwinch_handler(int sig) 
{
	get_terminal_size(&state.screen_rows, &state.screen_cols);
	screen_clear();
	fflush(stdout);

}

void scroll() 
{
	if (state.cursor_y >= state.row_offset + state.screen_rows - 1)
	{
		state.row_offset = state.cursor_y - state.screen_rows + 2;
	}

	if (state.cursor_y < state.row_offset)
	{
		state.row_offset = state.cursor_y;
	}	

	if (state.cursor_x >= state.row_offset + state.screen_cols)
	{
		state.col_offset = state.cursor_x - state.screen_cols + 1;
	}

	if (state.cursor_x < state.row_offset)
	{
		state.col_offset = state.cursor_x;
	}
}

void editorLoop()
{
	// Initialize state
	state.row_offset = 0;
	state.col_offset = 0;
	state.cursor_x = 0;
	state.cursor_y = 0;
	state.mode = NORMAL;
	get_terminal_size(&state.screen_rows, &state.screen_cols);
	signal(SIGWINCH, sigwinch_handler);

	GapBuffer *buffer = buffer_create(1024);

	while (1)
	{
		printf("\x1b[2J");
		printf("\x1b[H");

		render_text(buffer, state.row_offset, state.screen_rows - 1, state.col_offset, state.screen_cols);
		draw_status_line(state.cursor_x, state.cursor_y, state.screen_rows, state.mode);

		printf("\x1b[%d;%dH", state.cursor_y + 1, state.cursor_x + 1);

		fflush(stdout);

		char c;
		read(STDIN_FILENO, &c, 1);

		if (state.mode == NORMAL)
		{
    
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

		else if (c == 'i')
		{
			state.mode = INSERT;
		}

	}

		else if (state.mode == INSERT)
		{
			// If ESC is clicked switch to NORMAL mode
			if (c == 27)
			{
				state.mode = NORMAL;
			}

			// If BACKSPACE is clicked delete the character
			if (c == 127)
			{
				buffer_delete_char(buffer);
			}

			// If Character is to be inserted insert the character
			if (c >= 32 && c <= 126)
			{
				buffer_insert_char(buffer, c);
			}
		}

		scroll();
		
		/* Print AFTER updating state
		printf("Key: %c | Cursor: (%d, %d)\r\n", c, state.cursor_x, state.cursor_y);
		fflush(stdout);
	*/
	} 

}
