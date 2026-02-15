#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "buffer.h"
#include <sys/types.h>

GapBuffer* buffer_create(size_t initial_size) {

	GapBuffer *buffer = malloc(sizeof(GapBuffer));

	if(buffer == NULL) {
		return NULL;
	}

	buffer->data = malloc(sizeof(char) * initial_size);
	if(buffer->data ==NULL) {
		free(buffer);
		return NULL;
	}

	buffer->gap_start = 0;
	buffer->gap_end = initial_size;
	buffer->capacity = initial_size;

	return buffer;

}

int buffer_cursor_to_index(GapBuffer *buffer, int cursor_pos) 
{
	if (cursor_pos < buffer->gap_start) 
	{
		return cursor_pos;
	}

	else 
	{
		return cursor_pos + (buffer->gap_end - buffer->gap_start);
	}
}

void buffer_insert_char(GapBuffer *buffer, char c) 
{
	if (buffer->gap_start == buffer->gap_end) 
	{
		buffer_grow(buffer);
	}

	buffer->data[buffer->gap_start] = c;

	buffer->gap_start++;
}

void buffer_delete_char(GapBuffer *buffer)
{
	if (buffer->gap_start == 0)
	{
		return;
	}

	buffer->gap_start--;
}

void buffer_move_cursor_right(GapBuffer *buffer)
{
	if (buffer->gap_end >= buffer->capacity) 
	{
		return;
	}

	buffer->data[buffer->gap_start] = buffer->data[buffer->gap_end];

	buffer->gap_start++;
	buffer->gap_end++;
}

void buffer_move_cursor_left(GapBuffer *buffer)
{
	if (buffer->gap_start == 0)
	{
		return;
	}

	buffer->gap_start--;
	buffer->gap_end--;

	buffer->data[buffer->gap_end] = buffer->data[buffer->gap_start];
}

void buffer_grow(GapBuffer *buffer) 
{
	size_t old_capacity = buffer->capacity;
	size_t old_gap_end = buffer->gap_end;

	size_t new_capacity = buffer->capacity * 2;
	
	char *new_data = realloc(buffer->data, new_capacity * sizeof(char));

	if(new_data == NULL) 
	{
		return;
	}

	buffer->data = new_data;

	size_t chars_to_move = old_capacity - old_gap_end;
	size_t new_gap_end = new_capacity - chars_to_move;

	memmove(&buffer->data[new_gap_end],
	       &buffer->data[old_gap_end],
	       chars_to_move);

	buffer->gap_end = new_gap_end;
	buffer->capacity = new_capacity;
	
}

void buffer_print_debug(GapBuffer *buffer) {
    printf("Buffer contents: [");
    for (size_t i = 0; i < buffer->capacity; i++) {
        if (i >= buffer->gap_start && i < buffer->gap_end) {
            printf("_");  // Show gap as underscore
        } else {
            printf("%c", buffer->data[i]);
        }
    }
    printf("]\n");
    printf("gap_start=%zu, gap_end=%zu, capacity=%zu\n", 
           buffer->gap_start, buffer->gap_end, buffer->capacity);
}

size_t buffer_get_line_length(GapBuffer *buffer, size_t line_number)
{
	size_t current_row = 0;
	size_t column_count = 0;

	for (size_t i = 0; i < buffer->capacity; i++)
	{
		if (i >= buffer->gap_start && i < buffer->gap_end)
		{
			continue;
		}

		char c = buffer->data[i];
		
		if (current_row == line_number)
		{
    			// We're on the line we want!
    			// Check if this character is newline
    			if (c == '\n')
    			{
        			// Found end of line - return the count
        			return column_count;
    			}
    			else
    			{
        		// Still on the line - count this character
        		column_count++;
    			}
		}
		
		else
		{
    			// We're on a different row
    			// Check if newline (move to next row)
    			if (c == '\n')
    			{
        		current_row++;
    			}
		}
	}

	return column_count;

}

size_t buffer_get_total_lines(GapBuffer *buffer)
{
	size_t line_count = 1;

	for (size_t i = 0; i < buffer->capacity; i++)
	{
		if (i >= buffer->gap_start && i < buffer->gap_end)
                 {
                         continue;
                 }

		// Count \n amount of characters

		char c = buffer->data[i];

		if (c == '\n')
		{
			line_count++;
		}
	}

	return line_count;
}

ssize_t buffer_find_pattern(GapBuffer *buffer, char *pattern, size_t start_pos)
{
	size_t pattern_len = strlen(pattern);
	if (pattern_len == 0) return -1;

	size_t match_count = 0;
	size_t match_start = 0;

	for (size_t i = start_pos; i < buffer->capacity; i++)
	{
		if (i >= buffer->gap_start && i < buffer->gap_end)
		{
			continue;
		}

		char c = buffer->data[i];

		// Check if character matches pattern
		if (c == pattern[match_count])
		{
			if (match_count == 0)
			{
				match_start = i;
			}
			match_count++;

			if (match_count == pattern_len)
			{
				return match_start;
			}
		}
		else 
		{
			match_count = 0;
		}		
	}

	return -1;
}

void buffer_index_to_screen(GapBuffer *buffer, size_t index, size_t *row, size_t *col)
{
    *row = 0;
    *col = 0;
    
    for (size_t i = 0; i < index && i < buffer->capacity; i++)
    {
        // Skip gap
        if (i >= buffer->gap_start && i < buffer->gap_end)
        {
            continue;
        }
        
        char c = buffer->data[i];
        
        if (c == '\n')
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

ssize_t buffer_find_pattern_backward(GapBuffer *buffer, char *pattern, size_t start_pos)
{
	size_t pattern_len = strlen(pattern);
    if (pattern_len == 0) return -1;

	// Start from the END of buffer
    // Note: capacity is size_t (unsigned), so we need to be careful with the loop
    for (size_t i = start_pos + 1; i > 0; i--)
    {
        size_t index = i - 1;  // Actual index (since i starts at capacity)
        
        // Skip gap
        if (index >= buffer->gap_start && index < buffer->gap_end)
        {
            continue;
        }
        
        // Now check if pattern ENDS at this position
        // Your matching logic here...
		// At position 'index', check if pattern ENDS here
		// We need to check backwards by pattern_len

		// Make sure we have enough space before this position
		if (index < pattern_len - 1)
		{
			continue;  // Not enough characters before this position
		}

		// Now check each character of the pattern
		bool match = true;
		for (size_t j = 0; j < pattern_len; j++)
		{
			size_t check_pos = index - (pattern_len - 1) + j;
			
			// Skip if check_pos is in gap
			if (check_pos >= buffer->gap_start && check_pos < buffer->gap_end)
			{
				match = false;
				break;
			}
			
			// Check if character matches
			if (buffer->data[check_pos] != pattern[j])
			{
				match = false;
				break;
			}
		}

		if (match)
		{
			// Found it! Return the START of the match
			return index - (pattern_len - 1);
		}

    }

    return -1;
}

size_t buffer_screen_to_index(GapBuffer *buffer, size_t target_row, size_t target_col)
{
    size_t current_row = 0;
    size_t current_col = 0;
    
    for (size_t i = 0; i < buffer->capacity; i++)
    {
        // Skip gap
        if (i >= buffer->gap_start && i < buffer->gap_end)
        {
            continue;
        }
        
        // Check if we've reached target
        if (current_row == target_row && current_col == target_col)
        {
            return i;
        }
        
        char c = buffer->data[i];
        
        if (c == '\n')
        {
            current_row++;
            current_col = 0;
        }
        else
        {
            current_col++;
        }
    }
    
    return buffer->capacity;  // Return end if not found
}
