#ifndef SCREEN_RENDER
#define SCREEN_RENDER

#include "buffer.h"

void screen_clear(void);
void render_text(GapBuffer *buffer, size_t row_offset, size_t screen_rows);
void render_get_cursor_pos(GapBuffer *buffer, size_t *row, size_t *col);

#endif
