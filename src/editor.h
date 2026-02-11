#ifndef EDITOR_H
#define EDITOR_H

typedef enum 
{
	ACTION_INSERT, 
	ACTION_DELETE

} ActionType;

typedef struct Action
{
	ActionType type;
	char *text;
	size_t cursor_pos;
	size_t length;
	struct Action *next;
} Action;

typedef enum 
{
	NORMAL,
	INSERT,
	COMMAND,

} EditorMode;

typedef struct {
    int cursor_x;     // Cursor column position
    int cursor_y;     // Cursor row position
    size_t screen_rows;  // Terminal height
    size_t screen_cols;  // Terminal width
    size_t row_offset; // Line in the file at the top of the screen
    size_t col_offset;
    EditorMode mode;
    char *message;
    char command_buffer[256];
    size_t command_length;
    Action *undo_stack;
    Action *redo_stack;
    char *insert_buffer;
    size_t insert_start;
} EditorState;


void editorLoop(char *filename);

#endif
