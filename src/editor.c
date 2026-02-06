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

		// Check for arrow keys first
		if (c == 27)
		{
			char seq[2];
			
			if (read(STDIN_FILENO, &seq[0], 1) == 1) 
			{
				if (seq[0] == '[')
				{
					if(read(STDIN_FILENO, &seq[1], 1) ==1) 
					{
						if(seq[1] == 'A') 
						{
							 if(state.cursor_y > 0)
                                			 {
                                        			state.cursor_y--;
                                			 }	
						}

						else if (seq[1] == 'B') 
						{
							 if(state.cursor_y < state.screen_rows - 1)
                                			 {
                                        			state.cursor_y++;

                                			 }	
						}

						else if (seq[1] == 'C')
						{
							  if (state.cursor_x < state.screen_cols - 1)
                                			  {
                                        			state.cursor_x++;
                                			  }
						}

						else if (seq[1] == 'D')
						{
							if (state.cursor_x > 0)
                                			{
                                        			state.cursor_x--;
                                			}
						}

						else if (seq[1] == 'H')
						{
							state.cursor_x = 0;
							scroll();
							continue;
						}

						else if (seq[1] == '1')
						{
							char seq2;
							if (read(STDIN_FILENO, &seq2, 1) == 1) 
							{
								if (seq2 == '~')
								{
									state.cursor_x = 0;
									scroll();
									continue;
								}
							}
						}

						else if (seq[1] == 'F') 
						{
    							// End key: ESC [ F
    							size_t line_length = buffer_get_line_length(buffer, state.cursor_y);
    							state.cursor_x = line_length;
    							scroll();
    							continue;
						}

						else if (seq[1] == '4') 
						{
    							// Might be End key (ESC [ 4 ~)
    							char seq2;
    							if (read(STDIN_FILENO, &seq2, 1) == 1) 
							{
        							if (seq2 == '~') 
								{
            								// It's End key!
            								size_t line_length = buffer_get_line_length(buffer, state.cursor_y);
            								state.cursor_x = line_length;
            								scroll();
            								continue;
        							}
    							 }
						 }
					}

					scroll();
					continue;
				}

				
			}
		}

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

			else if (c == '0') 
			{
				state.cursor_x = 0;
			}

			else if (c == '$')
			{
				size_t line_length = buffer_get_line_length(buffer, state.cursor_y);
    				state.cursor_x = line_length;
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
