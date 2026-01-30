#ifndef EDITOR_H
#define EDITOR_H

typedef struct {
    int cursor_x;     // Cursor column position
    int cursor_y;     // Cursor row position
    int screen_rows;  // Terminal height
    int screen_cols;  // Terminal width
} EditorState;


void editorLoop();

#endif
