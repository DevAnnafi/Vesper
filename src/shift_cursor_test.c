#include <stdio.h>
#include "buffer.h"

int main() {
    GapBuffer *buf = buffer_create(10);
    
    // Insert "Hello"
    buffer_insert_char(buf, 'H');
    buffer_insert_char(buf, 'e');
    buffer_insert_char(buf, 'l');
    buffer_insert_char(buf, 'l');
    buffer_insert_char(buf, 'o');
    
    printf("After inserting 'Hello'\n");
    printf("gap_start: %zu, gap_end: %zu\n", buf->gap_start, buf->gap_end);
    
    // Move cursor left twice
    buffer_move_cursor_left(buf);
    buffer_move_cursor_left(buf);
    
    printf("\nAfter moving left twice\n");
    printf("gap_start: %zu, gap_end: %zu\n", buf->gap_start, buf->gap_end);
    
    // Insert 'X' in the middle
    buffer_insert_char(buf, 'X');
    
    printf("\nAfter inserting 'X'\n");
    printf("gap_start: %zu, gap_end: %zu\n", buf->gap_start, buf->gap_end);
    
    // Move right
    buffer_move_cursor_right(buf);
    
    printf("\nAfter moving right once\n");
    printf("gap_start: %zu, gap_end: %zu\n", buf->gap_start, buf->gap_end);

    return 0;
}
