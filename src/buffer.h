#ifndef GAP_BUFFER
#define GAP_BUFFER

#include <stddef.h>
#include <sys/types.h>

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
void buffer_move_cursor_left(GapBuffer *buffer);
void buffer_move_cursor_right(GapBuffer *buffer);
void buffer_grow(GapBuffer *buffer);
void buffer_print_debug(GapBuffer *buffer);
size_t buffer_get_line_length(GapBuffer *buffer, size_t line_number);
size_t buffer_get_total_lines(GapBuffer *buffer);
ssize_t buffer_find_pattern(GapBuffer *buffer, char *patter, size_t start_pos);
void buffer_index_to_screen(GapBuffer *buffer, size_t index, size_t *row, size_t *col);
ssize_t buffer_find_pattern_backward(GapBuffer *buffer, char *pattern, size_t start_pos);
size_t buffer_screen_to_index(GapBuffer *buffer, size_t target_row, size_t target_col);

#endif
