#ifndef EDITOR_H
#define EDITOR_H
#include <stdbool.h>

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
	OP_INSERT,
	OP_DELETE

} OpType;

typedef struct 
{
	OpType type;
	char *content;
	size_t position;
	size_t cursor_x;
	size_t cursor_y;

} UndoOperation;

typedef struct 
{
	UndoOperation *undo_stack;
	size_t undo_count;
	size_t undo_capacity;

	UndoOperation *redo_stack;
	size_t redo_count;
	size_t redo_capacity;

	char *current_insert_buffer;
	size_t current_insert_len;
	size_t current_insert_capacity;
	size_t insert_start_pos;
	size_t insert_start_x;
	size_t insert_start_y;
	bool in_insert_session;
} UndoManager;
	
typedef enum 
{
	NORMAL,
	INSERT,
	COMMAND,
	SEARCH

} EditorMode;

typedef struct {
    size_t cursor_x;     // Cursor column position
    size_t cursor_y;     // Cursor row position
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
    UndoManager *undo_manager;
    char search_buffer[256];
    size_t search_length;
	bool search_forward;
	char last_search_pattern[256];
	bool last_search_forward;
} EditorState;


void editorLoop(char *filename);

#endif
