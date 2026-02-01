#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"

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
