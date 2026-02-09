#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
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

        if (state.cursor_x >= state.col_offset + state.screen_cols)
        {
                state.col_offset = state.cursor_x - state.screen_cols + 1;
        }

        if (state.cursor_x < state.col_offset)
        {
                state.col_offset = state.cursor_x;
        }
}

void save_file(char *filename, GapBuffer *buffer, EditorState *state)
{
	if (filename == NULL) 
	{
		state->message = "Error: No filename";
		return;
	}

	FILE *fp = fopen(filename, "w");

	if (fp == NULL)
	{
		state->message = "Error: Cannot write file";
		return;
	}

	for(size_t i = 0; i < buffer->capacity; i++)
	{		
		if(i >= buffer->gap_start && i < buffer->gap_end)
		{
			continue;
		}

		char c = buffer->data[i];
		fputc(c, fp);

	}

	fclose(fp);
	state->message = "File saved!";
}

void editorLoop(char *filename)
{

	char *current_filename = filename;

        // Initialize state
        state.row_offset = 0;
        state.col_offset = 0;
        state.cursor_x = 0;
        state.cursor_y = 0;
        state.mode = NORMAL;
	state.message = NULL;
	state.command_buffer[0] = 0;
	state.command_length = 0;
        get_terminal_size(&state.screen_rows, &state.screen_cols);
        signal(SIGWINCH, sigwinch_handler);

        GapBuffer *buffer = buffer_create(1024);

	if (filename != NULL)
	{
		FILE *fp = fopen(filename, "r");

		if (fp != NULL)
		{
			fseek(fp, 0, SEEK_END);
			long file_size = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			char *contents = malloc(file_size + 1);

			if(contents != NULL)
			{	

				fread(contents, 1, file_size, fp);
				contents[file_size] = '\0';

				for (size_t i = 0; i < file_size; i++)
				{
					buffer_insert_char(buffer, contents[i]);
				}

				free(contents);
			}

			fclose(fp);
		}
	}



        while (1)
        {
                printf("\x1b[2J");
                printf("\x1b[H");

                render_text(buffer, state.row_offset, state.screen_rows - 1, state.col_offset, state.screen_cols);
                draw_status_line(state.cursor_x, state.cursor_y, state.screen_rows, state.mode, state.message, state.command_buffer);

                printf("\x1b[%d;%dH", state.cursor_y + 1, state.cursor_x + 1);

                fflush(stdout);

                char c;
                read(STDIN_FILENO, &c, 1);

		if (c == 19)
		{
			save_file(filename, buffer, &state);
			scroll();
			continue;
		}

                // Check for arrow keys first
                if (c == 27)
                {
                        char seq[2];

                        if (read(STDIN_FILENO, &seq[0], 1) == 1)
                        {
                                if (seq[0] == '[')
                                {
                                        if(read(STDIN_FILENO, &seq[1], 1) == 1)
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
                                                else if (seq[1] == '5')
                                                {
                                                        // Might be Page Up (ESC [ 5 ~)
                                                        char seq2;
                                                        if (read(STDIN_FILENO, &seq2, 1) == 1)
                                                        {
                                                                if (seq2 == '~')
                                                                {
                                                                        // It's Page Up!
                                                                        if (state.cursor_y < state.screen_rows - 1)
                                                                        {
                                                                                 state.cursor_y = 0;
                                                                        }
                                                                        else
                                                                        {
                                                                                state.cursor_y -= (state.screen_rows - 1);
                                                                        }
                                                                        scroll();
                                                                        continue;
                                                                }
                                                        }
                                                }
                                                else if (seq[1] == '6')
                                                {
                                                        // Might be Page Down (ESC [ 6 ~)
                                                        char seq2;
                                                        if (read(STDIN_FILENO, &seq2, 1) == 1)
                                                        {
                                                                if (seq2 == '~')
                                                                {
                                                                        // It's Page Down!
                                                                        size_t total_lines = buffer_get_total_lines(buffer);
                                                                        size_t new_pos = state.cursor_y + (state.screen_rows - 1);

                                                                        if (new_pos >= total_lines)
                                                                        {
                                                                                state.cursor_y = total_lines - 1;
                                                                        }
                                                                        else
                                                                        {
                                                                                state.cursor_y = new_pos;
                                                                        }
                                                                        scroll();
                                                                        continue;
                                                                 }
                                                        }
                                                }

                                                scroll();
                                                continue;
                                        }
                                }
                        }
                }

                if (state.mode == NORMAL)
                {
                        if (c == 'q')
                        {
                                break;
                        }
			else if (c == ':')
			{
				state.mode = COMMAND;
				state.command_buffer[0] = '\0';
				state.command_length = 0;
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
                        else if (c == 127)
                        {
                                buffer_delete_char(buffer);
                        }
			else if (c == 13 || c == 10)
			{
				buffer_insert_char(buffer, '\n');
			}
                        // If Character is to be inserted insert the character
                        else if (c >= 32 && c <= 126)
                        {
                                buffer_insert_char(buffer, c);
                        }
                }

		else if (state.mode == COMMAND)
		{
			if (c == 27)
			{
				state.mode = NORMAL;
				state.command_buffer[0] = '\0';
				state.command_length = 0;
				state.message = NULL;
			}

			else if (c == 127)
			{
				if (state.command_length > 0)
				{
					state.command_length--;

					// add null terminator at current_length position
					state.command_buffer[state.command_length] = '\0';
				}


			}

			else if (c == 13 || c == 10)
			{
				state.mode = NORMAL;
			}

			else if (c >= 32 && c <= 126)
			{
				if (state.message != NULL && state.message[0] != '\0')
				{
				}

				else if (state.command_length < 255)
				{
					state.command_buffer[state.command_length] = c;
					state.command_length++;
					state.command_buffer[state.command_length] = '\0';
				}

				else 
				{
					state.command_buffer[0] = '\0';
					state.command_length = 0;

					state.message = "Command too long";
				}

			}
		}

                scroll();
        }

}
