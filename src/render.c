#include <stdio.h>
#include "render.h"

void screen_clear(void)
{
	printf("\x1b[2J\x1b[H");
}
