#include <stdio.h>
#include "render.h"
#include "buffer.h"

int main() {
    GapBuffer *buf = buffer_create(20);
    
    // Insert "Hello World"
    buffer_insert_char(buf, 'H');
    buffer_insert_char(buf, 'e');
    buffer_insert_char(buf, 'l');
    buffer_insert_char(buf, 'l');
    buffer_insert_char(buf, 'o');
    buffer_insert_char(buf, ' ');
    buffer_insert_char(buf, 'W');
    buffer_insert_char(buf, 'o');
    buffer_insert_char(buf, 'r');
    buffer_insert_char(buf, 'l');
    buffer_insert_char(buf, 'd');
    
    printf("Rendering buffer:\n");
    render_text(buf);
    printf("\n");
    
    // Move cursor and insert in middle
    buffer_move_cursor_left(buf);
    buffer_move_cursor_left(buf);
    buffer_move_cursor_left(buf);
    buffer_insert_char(buf, 'X');
    
    printf("\nAfter inserting 'X' in middle:\n");
    render_text(buf);
    printf("\n");
    
    return 0;
}