#include <stdio.h>
#include "buffer.h"

int main() {
    GapBuffer *buf = buffer_create(5);
    
    printf("=== Test 1: Initial state ===\n");
    buffer_print_debug(buf);
    
    // Insert "Hello"
    buffer_insert_char(buf, 'H');
    buffer_insert_char(buf, 'e');
    buffer_insert_char(buf, 'l');
    buffer_insert_char(buf, 'l');
    buffer_insert_char(buf, 'o');
    
    printf("\n=== Test 2: After 'Hello' ===\n");
    buffer_print_debug(buf);
    
    // Trigger grow
    buffer_insert_char(buf, '!');
    
    printf("\n=== Test 3: After '!' (buffer grew) ===\n");
    buffer_print_debug(buf);
    
    // Move cursor to middle and insert
    buffer_move_cursor_left(buf);
    buffer_move_cursor_left(buf);
    buffer_move_cursor_left(buf);
    
    printf("\n=== Test 4: After moving left 3 times ===\n");
    buffer_print_debug(buf);
    
    buffer_insert_char(buf, 'X');
    
    printf("\n=== Test 5: After inserting 'X' in middle ===\n");
    buffer_print_debug(buf);
    printf("Expected text: 'HelXlo!'\n");
    
    // Move right and insert more
    buffer_move_cursor_right(buf);
    buffer_move_cursor_right(buf);
    buffer_move_cursor_right(buf);
    buffer_move_cursor_right(buf);
    
    printf("\n=== Test 6: After moving right 4 times ===\n");
    buffer_print_debug(buf);
    
    buffer_insert_char(buf, ' ');
    buffer_insert_char(buf, 'Y');
    
    printf("\n=== Test 7: After inserting ' Y' at end ===\n");
    buffer_print_debug(buf);
    printf("Expected text: 'HelXlo! Y'\n");
    
    // Delete some characters
    buffer_delete_char(buf);
    buffer_delete_char(buf);
    
    printf("\n=== Test 8: After deleting 2 chars ===\n");
    buffer_print_debug(buf);
    printf("Expected text: 'HelXlo!'\n");
    
    return 0;
}