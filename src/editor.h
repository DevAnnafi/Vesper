#ifndef EDITOR_H
#define EDITOR_H

typedef struct {
    int cursor_x;     // Cursor column position
    int cursor_y;     // Cursor row position
    size_t screen_rows;  // Terminal height
    size_t screen_cols;  // Terminal width
    size_t row_offset; // Line in the file at the top of the screen
} EditorState;


void editorLoop();

#endif
