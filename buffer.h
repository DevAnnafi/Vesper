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
