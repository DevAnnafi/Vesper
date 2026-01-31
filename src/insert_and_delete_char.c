#include <stdio.h>
#include "buffer.h"

int main() {
    GapBuffer *buf = buffer_create(10);
    
    printf("Created buffer\n");
    printf("gap_start: %zu, gap_end: %zu\n", buf->gap_start, buf->gap_end);
    
    // Insert some characters
    buffer_insert_char(buf, 'H');
    buffer_insert_char(buf, 'e');
    buffer_insert_char(buf, 'l');
    buffer_insert_char(buf, 'l');
    buffer_insert_char(buf, 'o');
    
    printf("\nAfter inserting 'Hello'\n");
    printf("gap_start: %zu, gap_end: %zu\n", buf->gap_start, buf->gap_end);
    
    // Delete a character
    buffer_delete_char(buf);
    printf("\nAfter deleting one char\n");
    printf("gap_start: %zu, gap_end: %zu\n", buf->gap_start, buf->gap_end);
    
    // Try deleting all
    buffer_delete_char(buf);
    buffer_delete_char(buf);
    buffer_delete_char(buf);
    buffer_delete_char(buf);
    printf("\nAfter deleting all\n");
    printf("gap_start: %zu, gap_end: %zu\n", buf->gap_start, buf->gap_end);
    
    // Try deleting when empty (should do nothing)
    buffer_delete_char(buf);
    printf("\nAfter trying to delete past beginning\n");
    printf("gap_start: %zu, gap_end: %zu\n", buf->gap_start, buf->gap_end);
    
    return 0;
}
