#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>

void enableRawMode();
void disableRawMode();
void get_terminal_size(size_t *rows, size_t *cols);

#endif
