#include <stdio.h>
#include "render.h"
#include "buffer.h"

void screen_clear(void)
{
	printf("\x1b[2J\x1b[H");
}

void render_text(GapBuffer *buffer, size_t row_offset, size_t screen_rows)
{
	size_t current_row = 0;

	for (size_t i = 0; i < buffer->capacity; i++)
	{
		if (i >= buffer->gap_start &&  i < buffer->gap_end)
		{
			continue;
		}

		// Get the character
		char c = buffer->data[i];

		// Only print if current_row is in visible range
		if (current_row >= row_offset && current_row < row_offset + screen_rows)
		{
			printf("%c", c);
		}

		 if (c == '\n')
		{
			current_row++;
		}

			if (current_row >= row_offset + screen_rows)
			{
				break;
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
