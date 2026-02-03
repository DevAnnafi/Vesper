#include <stdio.h>
#include "buffer.h"
#include "render.h"

int main() {
    GapBuffer *buf = buffer_create(50);
    size_t row, col;

    // Insert "Hello\nWorld\nTe"
    char *text = "Hello\nWorld\nTe";
    for (size_t i = 0; text[i]; i++) {
        buffer_insert_char(buf, text[i]);
    }

    render_get_cursor_pos(buf, &row, &col);
    printf("Test 1 - After 'Hello\\nWorld\\nTe':\n");
    printf("row: %zu, col: %zu\n", row, col);
    printf("Expected: row=2, col=2\n\n");

    // Insert more on same line
    buffer_insert_char(buf, 'x');
    buffer_insert_char(buf, 't');

    render_get_cursor_pos(buf, &row, &col);
    printf("Test 2 - After adding 'xt':\n");
    printf("row: %zu, col: %zu\n", row, col);
    printf("Expected: row=2, col=4\n\n");

    // Add new line
    buffer_insert_char(buf, '\n');

    render_get_cursor_pos(buf, &row, &col);
    printf("Test 3 - After adding newline:\n");
    printf("row: %zu, col: %zu\n", row, col);
    printf("Expected: row=3, col=0\n");

    return 0;
}