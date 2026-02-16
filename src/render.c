#include <stdio.h>
#include <string.h>
#include "render.h"
#include "buffer.h"

void screen_clear(void)
{
    printf("\x1b[2J\x1b[H");
}

void render_text(GapBuffer *buffer, size_t row_offset, size_t screen_rows, size_t col_offset, size_t screen_cols, bool in_search_mode, char *search_pattern)
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
        if (current_row >= row_offset && current_row < row_offset + screen_rows && current_col >= col_offset && current_col < col_offset + screen_cols)
        {
            if (in_search_mode && search_pattern[0] != '\0')
            {
                bool match_found = false;
                size_t pattern_len = strlen(search_pattern);
                
                if (i + pattern_len <= buffer->capacity)
                {
                    match_found = true;
                    
                    for (size_t j = 0; j < pattern_len; j++)
                    {
                        size_t check_pos = i + j;
                        
                        if (check_pos >= buffer->gap_start && check_pos < buffer->gap_end)
                        {
                            match_found = false;
                            break;
                        }
                        
                        if (buffer->data[check_pos] != search_pattern[j])
                        {
                            match_found = false;
                            break;
                        }
                    }
                }
                
                if (match_found)
                {
                    printf("\x1b[43;30m");
                    
                    for (size_t j = 0; j < pattern_len; j++)
                    {
                        size_t print_pos = i + j;
                        printf("%c", buffer->data[print_pos]);
                    }
                    
                    printf("\x1b[0m");
                    
                    i += pattern_len - 1;
                    current_col += pattern_len;
                }
                else
                {
                    printf("%c", c);
                }
            }
            else
            {
                printf("%c", c);
            }
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

void draw_status_line(size_t cursor_x, size_t cursor_y, size_t screen_rows, EditorMode mode, char *message, char *command_buffer, char *search_buffer, bool search_forward)
{
    printf("\x1b[%zu;1H", screen_rows);
    printf("\x1b[7m");

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
        printf("%s%s", search_forward ? "/" : "?", search_buffer);
    }
    else 
    {
        printf(" -- NORMAL -- ");
    }

    if (mode != COMMAND && mode != SEARCH)
    {
        printf("Row: %zu, Col: %zu ", cursor_y, cursor_x);

        if (message != NULL && message[0] != '\0')
        {
            printf(" | %s", message);
        }
    }
    
    printf("\x1b[m");
    fflush(stdout);
}