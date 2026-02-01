#include <stdio.h>
#include "buffer.h"

int main() {
    // Create small buffer (capacity 5)
    GapBuffer *buf = buffer_create(5);
    
    printf("Initial state:\n");
    printf("capacity: %zu, gap_start: %zu, gap_end: %zu\n\n", 
           buf->capacity, buf->gap_start, buf->gap_end);
    
    // Fill the gap by inserting 5 characters
    buffer_insert_char(buf, 'H');
    buffer_insert_char(buf, 'e');
    buffer_insert_char(buf, 'l');
    buffer_insert_char(buf, 'l');
    buffer_insert_char(buf, 'o');
    
    printf("After inserting 'Hello' (gap should be full):\n");
    printf("capacity: %zu, gap_start: %zu, gap_end: %zu\n\n", 
           buf->capacity, buf->gap_start, buf->gap_end);
    
    // This should trigger buffer_grow
    buffer_insert_char(buf, '!');
    
    printf("After inserting '!' (should have grown):\n");
    printf("capacity: %zu, gap_start: %zu, gap_end: %zu\n\n", 
           buf->capacity, buf->gap_start, buf->gap_end);
    
    // Insert a few more to verify it still works
    buffer_insert_char(buf, ' ');
    buffer_insert_char(buf, 'W');
    
    printf("After inserting ' W':\n");
    printf("capacity: %zu, gap_start: %zu, gap_end: %zu\n", 
           buf->capacity, buf->gap_start, buf->gap_end);
    
    return 0;
}
