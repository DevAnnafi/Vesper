#include <stdlib.h>
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
	buffer->size = initial_size;

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
