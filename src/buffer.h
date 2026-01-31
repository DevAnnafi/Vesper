#ifndef GAP_BUFFER
#define GAP_BUFFER

#include <stddef.h>

typedef struct 
{
	char* data;
	size_t gap_start;
	size_t gap_end;
	size_t capacity;
} GapBuffer;

GapBuffer* buffer_create(size_t initial_size);
int buffer_cursor_to_index(GapBuffer* buffer, int cursor_pos);
void buffer_insert_char(GapBuffer *buffer, char c);
void buffer_delete_char(GapBuffer *buffer);

#endif
