#include <stdio.h>
#include "render.h"
#include "buffer.h"

void screen_clear(void)
{
	printf("\x1b[2J\x1b[H");
}

void render_text(GapBuffer *buffer) 
{

	for (size_t i = 0; i < buffer->capacity; i++)
	{
		if (i >= buffer->gap_start &&  i < buffer->gap_end)
		{
			continue;
		}

		else 
		{
			printf("%c", buffer->data[i]);
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
