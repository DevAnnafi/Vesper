#ifndef SCREEN_RENDER
#define SCREEN_RENDER

#include "buffer.h"

void screen_clear(void);
void render_text(GapBuffer *buffer);
void render_get_cursor_pos(GapBuffer *buffer, size_t *row, size_t *col);

#endif
