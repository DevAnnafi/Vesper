#include <stdio.h>
#include "render.h"
#include "buffer.h"

void screen_clear(void)
{
    printf("\x1b[2J\x1b[H");
}

void render_text(GapBuffer *buffer, size_t row_offset, size_t screen_rows, size_t col_offset, size_t screen_cols)
{
    size_t current_row = 0;
    size_t current_col = 0;
    
    for (size_t i = 0; i < buffer->capacity; i++)
    {
        if (i >= buffer->gap_start && i < buffer->gap_end)
        {
            continue;
        }
        
        // Get the character
        char c = buffer->data[i];
        
        // Only print if BOTH row AND column are in visible range
        if (current_row >= row_offset && current_row < row_offset + screen_rows &&
            current_col >= col_offset && current_col < col_offset + screen_cols)
        {
            printf("%c", c);
        }
        
        if (c == '\n')
        {
            current_row++;
            current_col = 0;
            
            if (current_row >= row_offset + screen_rows)
            {
                break;
            }
        }
        else
        {
            current_col++;
        }
    }
}

void render_get_cursor_pos(GapBuffer *buffer, size_t *row, size_t *col)
{
    *row = 0;
    *col = 0;
    
    for (size_t i = 0; i < buffer->gap_start; i++)
    {
        if (buffer->data[i] == '\n')
        {
            (*row)++;
            *col = 0;
        }
        else
        {
            (*col)++;
        }
    }
}

void draw_status_line(size_t cursor_x, size_t cursor_y, size_t screen_rows, EditorMode mode, char *message, char *command_buffer, char *search_buffer)
{
    // Move cursor to bottom row
    printf("\x1b[%zu;1H", screen_rows);
    
    // Invert colors for status line (optional but looks nice)
    printf("\x1b[7m");

    // Show mode
    if (mode == INSERT) 
    {
        printf(" -- INSERT -- ");
    } 
    
    else if (mode == COMMAND)
	{
		if (message != NULL && message[0] != '\0')
		{
			printf(":%s [ERROR: %s]", command_buffer, message);
		}
		else 
		{
			printf(":%s", command_buffer);
		}
	}

    else if (mode == SEARCH)
    {
	    printf("/%s", search_buffer);
    }

    else 
    {
        printf(" -- NORMAL -- ");
    }

    if (mode != COMMAND && mode != SEARCH)
    {

    	printf("Row: %zu, Col: %zu ", cursor_y, cursor_x);

    	// Show message if present
    	if (message != NULL && message[0] != '\0')
	{
		printf(" | %s", message);
	}
    }
    // Reset colors
    printf("\x1b[m");
    
    fflush(stdout);
}
