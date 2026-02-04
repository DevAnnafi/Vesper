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
