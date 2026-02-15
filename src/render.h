#ifndef SCREEN_RENDER
#define SCREEN_RENDER

#include "buffer.h"
#include "editor.h"

void screen_clear(void);
void render_text(GapBuffer *buffer, size_t row_offset, size_t screen_rows, size_t col_offset, size_t screen_cols);
void render_get_cursor_pos(GapBuffer *buffer, size_t *row, size_t *col);
void draw_status_line(size_t cursor_x, size_t cursor_y, size_t screen_rows, EditorMode mode, char *message, char *command_buffer, char *search_buffer, bool search_forward);

#endif
