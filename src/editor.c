#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include "terminal.h"
#include "editor.h"
#include "render.h"
#include "buffer.h"

EditorState state;

char *c_keywords[] = 
{
	"auto",
	"break",
	"case",
	"char",
	"const",
	"continue",
	"default",
	"do",
	"double",
	"else",
	"enum",
	"extern",
	"float",
	"for",
	"goto",
	"if",
	"int",
	"long",
	"register",
	"return",
	"short",
	"signed",
	"sizeof",
	"static",
	"struct",
	"switch",
	"typedef",
	"union",
	"unsigned",
	"void",
	"volatile",
	"while", 
	NULL
};


char *python_keywords[] = 
{
	"and", "as", "assert", "async", "await",
	"break", "class", "continue",
	"def", "del",
	"elif", "else", "except",
	"False", "finally", "for", "from",
	"global",
	"if", "import", "in", "is",
	"lambda",
	"None", "nonlocal", "not",
	"or",
	"pass",
	"raise", "return",
	"True", "try",
	"while", "with",
	"yield",
	NULL
};


char *javascript_keywords[] = 
{
	"abstract", "arguments", "await",
	"break",
	"case", "catch", "class", "const", "continue",
	"debugger", "default", "delete", "do",
	"else", "enum", "eval", "export", "extends",
	"false", "finally", "for", "function",
	"if", "implements", "import", "in", "instanceof", "interface",
	"let",
	"new", "null",
	"package", "private", "protected", "public",
	"return",
	"static", "super", "switch",
	"this", "throw", "true", "try", "typeof",
	"var", "void",
	"while", "with",
	"yield",
	NULL
};


char *java_keywords[] = 
{
	"abstract", "assert",
	"boolean", "break", "byte",
	"case", "catch", "char", "class", "const", "continue",
	"default", "do", "double",
	"else", "enum", "extends",
	"false", "final", "finally", "float", "for",
	"goto",
	"if", "implements", "import", "instanceof", "int", "interface",
	"long",
	"native", "new", "null",
	"package", "private", "protected", "public",
	"return",
	"short", "static", "strictfp", "super", "switch", "synchronized",
	"this", "throw", "throws", "transient", "true", "try",
	"void", "volatile",
	"while",
	NULL
};


char *go_keywords[] = 
{
	"break",
	"case", "chan", "const", "continue",
	"default", "defer",
	"else",
	"fallthrough", "for", "func",
	"go", "goto",
	"if", "import", "interface",
	"map",
	"package",
	"range", "return",
	"select", "struct", "switch",
	"type",
	"var",
	NULL
};


char *rust_keywords[] = 
{
	"as", "async", "await",
	"break",
	"const", "continue", "crate",
	"dyn",
	"else", "enum", "extern",
	"false", "fn", "for",
	"if", "impl", "in",
	"let", "loop",
	"match", "mod", "move", "mut",
	"pub",
	"ref", "return",
	"self", "Self", "static", "struct", "super",
	"trait", "true", "type",
	"unsafe", "use",
	"where", "while",
	"yield",
	NULL
};

bool is_c_keyword(char *word) 
{
	for (int i = 0; c_keywords[i] != NULL; i++)
	{
		if(strcmp(c_keywords[i], word) == 0)
		{
			return true;
		}
	}
	return false;
}

bool is_python_keyword(char *word) 
{
	for (int i = 0; python_keywords[i] != NULL; i++)
	{
		if(strcmp(python_keywords[i], word) == 0)
		{
			return true;
		}
	}
	return false;
}

bool is_java_keyword(char *word) 
{
	for (int i = 0; java_keywords[i] != NULL; i++)
	{
		if(strcmp(java_keywords[i], word) == 0)
		{
			return true;
		}
	}
	return false;
}

bool is_go_keyword(char *word) 
{
	for (int i = 0; go_keywords[i] != NULL; i++)
	{
		if(strcmp(go_keywords[i], word) == 0)
		{
			return true;
		}
	}
	return false;
}

bool is_rust_keyword(char *word) 
{
	for (int i = 0; rust_keywords[i] != NULL; i++)
	{
		if(strcmp(rust_keywords[i], word) == 0)
		{
			return true;
		}
	}
	return false;
}

bool is_javascript_keyword(char *word) 
{
	for (int i = 0; javascript_keywords[i] != NULL; i++)
	{
		if(strcmp(javascript_keywords[i], word) == 0)
		{
			return true;
		}
	}
	return false;
}

bool is_digit(char c)
{
	if (c >= '0' && c <= '9')
	{
		return true;
	}

	return false;
}

bool is_operator(char c)
{
	if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '<' || c == '>' || c == '=' || c == '!' || c == '&' || c == '|' || c == '^' || c == '~' || c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == ';' || c == ',' || c == '.') 
	{
		return true;
	}

	return false;
}

bool is_word_char(char c)
{
	if (c >= 'a' && c <= 'z')
	{
		return true;
	}

	if (c >= 'A' && c <= 'Z')
	{
		return true;
	}

	if (c >= '0' && c <= '9')
	{
		return true;
	}

	if (c == '_')
	{
		return true;
	}

	return false;
}

void extract_word(GapBuffer *buffer, size_t pos, char *word_buffer, size_t max_len)
{
	size_t word_start = pos;

	while (word_start > 0)
	{
		size_t check_pos = word_start - 1;
		
		// Skip if in gap
		if (check_pos >= buffer->gap_start && check_pos < buffer->gap_end)
		{
			word_start--;
			continue;
		}
		
		// Check if it's a word character
		if (is_word_char(buffer->data[check_pos]))
		{
			word_start--;
		}
		else
		{
			break;  // Found non-word character, stop!
		}
	}		

	size_t word_end = pos;

	while (word_end < buffer->capacity)
	{
		// Skip if in gap
		if (word_end >= buffer->gap_start && word_end < buffer->gap_end)
		{
			word_end++;
			continue;
		}
		
		// Check if it's a word character
		if (is_word_char(buffer->data[word_end]))
		{
			word_end++;
		}
		else
		{
			break;  // Found non-word character, stop!
		}
	}

	size_t word_len = 0;

	for (size_t i = word_start; i < word_end && word_len < max_len - 1; i++)
	{
		// Skip gap
		if (i >= buffer->gap_start && i < buffer->gap_end)
		{
			continue;
		}
		
		// Copy character
		word_buffer[word_len] = buffer->data[i];
		word_len++;
	}

	// Null terminate
	word_buffer[word_len] = '\0';
}

bool is_inside_line_comment(GapBuffer *buffer, size_t pos)
{
	size_t line_start = pos;

	while(line_start > 0) 
	{
		size_t check_pos = line_start - 1;
		
		// Skip if in gap
		if (check_pos >= buffer->gap_start && check_pos < buffer->gap_end)
		{
			line_start--;
			continue;
						
		}

		if (buffer->data[check_pos] == '\n')
		{
			break;
		}

		line_start--;
	}

	for (size_t i = line_start; i < pos; i++)
	{
		// Skip if current position is in gap
		if (i >= buffer->gap_start && i < buffer->gap_end)
		{
			continue;
		}

		if (buffer->data[i] == '/')
		{
			// Now we need to check the NEXT position (i+1)
			size_t next_pos = i + 1;
			
			// Make sure next_pos is not beyond our search range
			if (next_pos >= pos)
			{
				continue;  // Can't check beyond pos
			}
			
			// Skip if next position is in gap
			if (next_pos >= buffer->gap_start && next_pos < buffer->gap_end)
			{
				continue;  // Can't form "//" if second char is in gap
			}
			
			// Now check if next character is also '/'
			if (buffer->data[next_pos] == '/')
			{
				return true;  // Found "//" before pos!
			}
		}
	}

	return false;
}

bool is_inside_string(GapBuffer *buffer, size_t pos)
{
	size_t line_start = pos;

	while(line_start > 0) 
	{
		size_t check_pos = line_start - 1;
		
		// Skip if in gap
		if (check_pos >= buffer->gap_start && check_pos < buffer->gap_end)
		{
			line_start--;
			continue;
						
		}

		if (buffer->data[check_pos] == '\n')
		{
			break;
		}

		line_start--;
	}

	int quote_count = 0;

	for (size_t i = line_start; i < pos; i++)
	{
		if (i >= buffer->gap_start && i < buffer->gap_end)
		{
			continue;
		}

		if (buffer->data[i] == '"')
		{
			bool is_escaped = false;

			if (i > line_start)
			{
				size_t prev_pos = i - 1;

				// Make sure previous position is not in gap
				if (prev_pos < buffer->gap_start || prev_pos >= buffer->gap_end)
				{
					// Check if previous character is backslash
					if (buffer->data[prev_pos] == '\\')
					{
						is_escaped = true;
					}
				}
			}
			
			// If NOT escaped, count it
			if (!is_escaped)
			{
				quote_count++;
			}
		}
	}	

	return (quote_count % 2 == 1);
}

bool is_inside_block_comment(GapBuffer *buffer, size_t pos)
{
	for (size_t i = pos; i > 0; i--)
	{
		size_t check_pos = i - 1;

		if (check_pos >= buffer->gap_start && check_pos < buffer->gap_end)
		{
			continue;
		}

		size_t next_pos = check_pos + 1;

		// Make sure next_pos is not in gap and is valid
        if (next_pos >= buffer->capacity)
        {
            continue;
        }
        
        if (next_pos >= buffer->gap_start && next_pos < buffer->gap_end)
        {
            continue;
        }
        
        // Now check for "*/" (closing comment)
        if (buffer->data[check_pos] == '*' && buffer->data[next_pos] == '/')
        {
            return false;  // Found closing, we're outside
        }
        
        // Check for "/*" (opening comment)
        if (buffer->data[check_pos] == '/' && buffer->data[next_pos] == '*')
        {
            return true;  // Found opening, we're inside
        }
	}

	return false;
}

TokenType classify_token(GapBuffer *buffer, size_t pos, LanguageType lang)
{
	if (lang == LANG_NONE)
    {
        return NORMALTXT;  // Only highlight C for now
    }
    
    // Step 2: Check if inside string (PRIORITY)
    if (is_inside_string(buffer, pos))
    {
        return STRINGS;
    }
    
    // Step 3: Check if inside line comment
    if (is_inside_line_comment(buffer, pos))
    {
        return COMMENTS;
    }
    
    // Step 4: Check if inside block comment
    if (is_inside_block_comment(buffer, pos))
    {
        return COMMENTS;
    }

	// Check if position is in gap
	if (pos >= buffer->gap_start && pos < buffer->gap_end)
	{
		return NORMALTXT;  // Can't classify gap
	}

	char c = buffer->data[pos];

	if (is_digit(c))
	{
    	return NUMBERS;
	}

	if (is_operator(c))
	{
		return OPERATORS;
	}

	if (is_word_char(c))
	{
		char word[256];
		extract_word(buffer, pos, word, 256);
		
		// Check based on language
		bool is_keyword = false;
		
		if (lang == LANG_C && is_c_keyword(word))
		{
			is_keyword = true;
		}
		else if (lang == LANG_PYTHON && is_python_keyword(word))
		{
			is_keyword = true;
		}
		else if (lang == LANG_JAVA && is_java_keyword(word))
		{
			is_keyword = true;
		}
		else if (lang == LANG_GO && is_go_keyword(word))
		{
			is_keyword = true;
		}
		else if (lang == LANG_JAVASCRIPT && is_javascript_keyword(word))
		{
			is_keyword = true;
		}
		else if (lang == LANG_RUST && is_rust_keyword(word))
		{
			is_keyword = true;
		}

		if (is_keyword)
		{
			return KEYWORDS;
		}

		// It's a word, but not a keyword
		return NORMALTXT;
	}

	return NORMALTXT;
}

char* read_api_key()
{
    // Try environment variable first
    char *key = getenv("ANTHROPIC_API_KEY");
    if (key != NULL && strlen(key) > 0)
    {
        // Found in environment, return a copy
        return strdup(key);
    }
    
    // Try reading from config file
    char config_path[1024];
    snprintf(config_path, sizeof(config_path), "%s/.vesperrc", getenv("HOME"));
    
    FILE *fp = fopen(config_path, "r");
    if (fp != NULL)
    {
        char line[1024];
        while (fgets(line, sizeof(line), fp))
        {
            // Look for ANTHROPIC_API_KEY=...
            if (strncmp(line, "ANTHROPIC_API_KEY=", 18) == 0)
            {
                // Extract key (after the =)
                char *key_start = line + 18;
                
                // Remove newline if present
                char *newline = strchr(key_start, '\n');
                if (newline) *newline = '\0';
                
                fclose(fp);
                return strdup(key_start);
            }
        }
        fclose(fp);
    }
    
    // Not found anywhere
    return NULL;
}

void sigwinch_handler(int sig)
{
	get_terminal_size(&state.screen_rows, &state.screen_cols);
	screen_clear();
	fflush(stdout);
}

void scroll()
{
	if (state.cursor_y >= state.row_offset + state.screen_rows - 1)
	{
		state.row_offset = state.cursor_y - state.screen_rows + 2;
	}

	if (state.cursor_y < state.row_offset)
	{
		state.row_offset = state.cursor_y;
	}

	if (state.cursor_x >= state.col_offset + state.screen_cols)
	{
		state.col_offset = state.cursor_x - state.screen_cols + 1;
	}

	if (state.cursor_x < state.col_offset)
	{
		state.col_offset = state.cursor_x;
	}
}

void save_file(char *filename, GapBuffer *buffer, EditorState *state)
{
	if (filename == NULL)
	{
		state->message = "Error: No filename";
		return;
	}

	FILE *fp = fopen(filename, "w");

	if (fp == NULL)
	{
		state->message = "Error: Cannot write file";
		return;
	}

	for (size_t i = 0; i < buffer->capacity; i++)
	{
		if (i >= buffer->gap_start && i < buffer->gap_end)
		{
			continue;
		}

		char c = buffer->data[i];
		fputc(c, fp);
	}

	fclose(fp);
	state->message = "File saved!";
}

UndoManager *undo_manager_create()
{
	UndoManager *um = malloc(sizeof(UndoManager));

	um->undo_stack = malloc(sizeof(UndoOperation) * 10);
	um->undo_count = 0;
	um->undo_capacity = 10;

	um->redo_stack = malloc(sizeof(UndoOperation) * 10);
	um->redo_count = 0;
	um->redo_capacity = 10;

	um->current_insert_buffer = malloc(256); // Start with 256 chars
	um->current_insert_len = 0;
	um->current_insert_capacity = 256;
	um->insert_start_pos = 0;
	um->insert_start_x = 0;
	um->insert_start_y = 0;
	um->in_insert_session = false;

	return um;
}

void undo_push_operation(UndoManager *um, OpType type, char *content, size_t pos, size_t cx, size_t cy)
{
	// Grow stack if needed
	if (um->undo_count >= um->undo_capacity)
	{
		um->undo_capacity *= 2;
		um->undo_stack = realloc(um->undo_stack, sizeof(UndoOperation) * um->undo_capacity);
	}

	// Create operation
	UndoOperation op;
	op.type = type;
	op.content = malloc(strlen(content) + 1);
	strcpy(op.content, content);
	op.position = pos;
	op.cursor_x = cx;
	op.cursor_y = cy;

	// Add to stack
	um->undo_stack[um->undo_count] = op;
	um->undo_count++;

	// Clear redo stack (new action invalidates redo)
	um->redo_count = 0;
}

void undo_push_operation_no_clear(UndoManager *um, OpType type, char *content, size_t pos, size_t cx, size_t cy)
{
	// Grow stack if needed
	if (um->undo_count >= um->undo_capacity)
	{
		um->undo_capacity *= 2;
		um->undo_stack = realloc(um->undo_stack, sizeof(UndoOperation) * um->undo_capacity);
	}

	// Create operation
	UndoOperation op;
	op.type = type;
	op.content = malloc(strlen(content) + 1);
	strcpy(op.content, content);
	op.position = pos;
	op.cursor_x = cx;
	op.cursor_y = cy;

	// Add to stack
	um->undo_stack[um->undo_count] = op;
	um->undo_count++;

	// DON'T clear redo stack here!
}

void redo_push_operation(UndoManager *um, OpType type, char *content, size_t pos, size_t cx, size_t cy)
{
	if (um->redo_count >= um->redo_capacity)
	{
		um->redo_capacity *= 2;
		um->redo_stack = realloc(um->redo_stack, sizeof(UndoOperation) * um->redo_capacity);
	}

	UndoOperation op;
	op.type = type;
	op.content = malloc(strlen(content) + 1);
	strcpy(op.content, content);
	op.position = pos;
	op.cursor_x = cx;
	op.cursor_y = cy;

	um->redo_stack[um->redo_count] = op;
	um->redo_count++;
}

void undo_operation(UndoManager *um, GapBuffer *buffer, EditorState *state)
{
	if (um->undo_count == 0)
	{
		state->message = "Nothing to undo";
		return;
	}

	um->undo_count--;
	UndoOperation op = um->undo_stack[um->undo_count];

	if (op.type == OP_INSERT)
	{
		size_t len = strlen(op.content);

		for (size_t i = 0; i < len; i++)
		{
			buffer_delete_char(buffer);
		}

		state->cursor_x = op.cursor_x;
		state->cursor_y = op.cursor_y;
	}

	else if (op.type == OP_DELETE)
	{
		state->cursor_x = op.cursor_x;
		state->cursor_y = op.cursor_y;

		size_t len = strlen(op.content);

		for (size_t i = 0; i < len; i++)
		{
			char c = op.content[i];

			buffer_insert_char(buffer, c);
		}
	}

	state->cursor_x = op.cursor_x;
	state->cursor_y = op.cursor_y;

	redo_push_operation(um, op.type, op.content, op.position, op.cursor_x, op.cursor_y);
}

void redo_operation(UndoManager *um, GapBuffer *buffer, EditorState *state)
{
	if (um->redo_count == 0)
	{
		state->message = "Nothing to redo";
		return;
	}

	um->redo_count--;
	UndoOperation op = um->redo_stack[um->redo_count];

	state->cursor_x = op.cursor_x;
	state->cursor_y = op.cursor_y;

	if (op.type == OP_INSERT)
	{
		size_t len = strlen(op.content);

		for (size_t i = 0; i < len; i++)
		{
			char c = op.content[i];
			buffer_insert_char(buffer, c);
		}
	}

	else if (op.type == OP_DELETE)
	{
		size_t len = strlen(op.content);

		for (size_t i = 0; i < len; i++)
		{
			buffer_delete_char(buffer);
		}
	}

	undo_push_operation_no_clear(um, op.type, op.content, op.position, op.cursor_x, op.cursor_y);
}

LanguageType detect_language(char *filename)
{
	if (filename == NULL)
	{
		return LANG_NONE;
	}

	char *dot = strrchr(filename, '.');

	if (dot == NULL)
	{
		return LANG_NONE;
	}

	char *extension = dot + 1;

	if (strcmp(extension, "c") == 0 || strcmp(extension, "h") == 0)
	{
		return LANG_C;
	}
	else if (strcmp(extension, "py") == 0)
	{
		return LANG_PYTHON;
	}
	else if (strcmp(extension, "js") == 0)
	{
		return LANG_JAVASCRIPT;
	}
	else if (strcmp(extension, "java") == 0)
	{
		return LANG_JAVA;
	}
	else if (strcmp(extension, "go") == 0)
	{
		return LANG_GO;
	}
	else if (strcmp(extension, "rs") == 0)
	{
		return LANG_RUST;
	}
	else
	{
		return LANG_NONE;
	}
}

void editorLoop(char *filename)
{

	char *current_filename = filename;

	// Initialize state
	state.row_offset = 0;
	state.col_offset = 0;
	state.cursor_x = 0;
	state.cursor_y = 0;
	state.mode = NORMAL;
	state.message = NULL;
	state.command_buffer[0] = 0;
	state.command_length = 0;
	state.undo_stack = NULL;
	state.redo_stack = NULL;
	state.insert_buffer = NULL;
	state.insert_start = 0;
	state.undo_manager = undo_manager_create();
	state.search_buffer[0] = '\0';
	state.search_length = 0;
	state.search_forward = true;
	state.last_search_pattern[0] = '\0';
	state.last_search_forward = true;
	state.highlight_search = false;
	state.highlight_pattern[0] = '\0';
	get_terminal_size(&state.screen_rows, &state.screen_cols);
	signal(SIGWINCH, sigwinch_handler);
	state.api_key = read_api_key();

	if (state.api_key == NULL)
	{
		fprintf(stderr, "Warning: ANTHROPIC_API_KEY not found!\n");
		fprintf(stderr, "Set environment variable or create ~/.vesperrc\n");
	}

	GapBuffer *buffer = buffer_create(1024);

	state.language = detect_language(filename);

	if (filename != NULL)
	{
		FILE *fp = fopen(filename, "r");

		if (fp != NULL)
		{
			fseek(fp, 0, SEEK_END);
			long file_size = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			char *contents = malloc(file_size + 1);

			if (contents != NULL)
			{

				fread(contents, 1, file_size, fp);
				contents[file_size] = '\0';

				for (size_t i = 0; i < file_size; i++)
				{
					buffer_insert_char(buffer, contents[i]);
				}

				free(contents);
			}

			fclose(fp);
		}
	}
    
	while (1)
	{
		printf("\x1b[2J");
		printf("\x1b[H");

		render_text(buffer, state.row_offset, state.screen_rows - 1, state.col_offset, state.screen_cols, state.mode == SEARCH, state.search_buffer, state.language);
		draw_status_line(state.cursor_x, state.cursor_y, state.screen_rows, state.mode, state.message, state.command_buffer, state.search_buffer, state.search_forward);

		printf("\x1b[%zu;%zuH", state.cursor_y + 1, state.cursor_x + 1);

		fflush(stdout);

		char c;
		read(STDIN_FILENO, &c, 1);

		if (c == 19)
		{
			save_file(filename, buffer, &state);
			scroll();
			continue;
		}

		// Check for arrow keys first
		if (c == 27)
		{
			char seq[2];

			if (read(STDIN_FILENO, &seq[0], 1) == 1)
			{
				if (seq[0] == '[')
				{
					if (read(STDIN_FILENO, &seq[1], 1) == 1)
					{
						if (seq[1] == 'A')
						{
							if (state.cursor_y > 0)
							{
								state.cursor_y--;
							}
						}
						else if (seq[1] == 'B')
						{
							if (state.cursor_y < state.screen_rows - 1)
							{
								state.cursor_y++;
							}
						}
						else if (seq[1] == 'C')
						{
							if (state.cursor_x < state.screen_cols - 1)
							{
								state.cursor_x++;
							}
						}
						else if (seq[1] == 'D')
						{
							if (state.cursor_x > 0)
							{
								state.cursor_x--;
							}
						}
						else if (seq[1] == 'H')
						{
							state.cursor_x = 0;
							scroll();
							continue;
						}
						else if (seq[1] == '1')
						{
							char seq2;
							if (read(STDIN_FILENO, &seq2, 1) == 1)
							{
								if (seq2 == '~')
								{
									state.cursor_x = 0;
									scroll();
									continue;
								}
							}
						}
						else if (seq[1] == 'F')
						{
							// End key: ESC [ F
							size_t line_length = buffer_get_line_length(buffer, state.cursor_y);
							state.cursor_x = line_length;
							scroll();
							continue;
						}
						else if (seq[1] == '4')
						{
							// Might be End key (ESC [ 4 ~)
							char seq2;
							if (read(STDIN_FILENO, &seq2, 1) == 1)
							{
								if (seq2 == '~')
								{
									// It's End key!
									size_t line_length = buffer_get_line_length(buffer, state.cursor_y);
									state.cursor_x = line_length;
									scroll();
									continue;
								}
							}
						}
						else if (seq[1] == '5')
						{
							// Might be Page Up (ESC [ 5 ~)
							char seq2;
							if (read(STDIN_FILENO, &seq2, 1) == 1)
							{
								if (seq2 == '~')
								{
									// It's Page Up!
									if (state.cursor_y < state.screen_rows - 1)
									{
										state.cursor_y = 0;
									}
									else
									{
										state.cursor_y -= (state.screen_rows - 1);
									}
									scroll();
									continue;
								}
							}
						}
						else if (seq[1] == '6')
						{
							// Might be Page Down (ESC [ 6 ~)
							char seq2;
							if (read(STDIN_FILENO, &seq2, 1) == 1)
							{
								if (seq2 == '~')
								{
									// It's Page Down!
									size_t total_lines = buffer_get_total_lines(buffer);
									size_t new_pos = state.cursor_y + (state.screen_rows - 1);

									if (new_pos >= total_lines)
									{
										state.cursor_y = total_lines - 1;
									}
									else
									{
										state.cursor_y = new_pos;
									}
									scroll();
									continue;
								}
							}
						}

						scroll();
						continue;
					}
				}
			}
		}

		if (state.mode == NORMAL)
		{
			if (c == 'q')
			{
				break;
			}
			else if (c == '/')
			{
				state.mode = SEARCH;
				state.search_buffer[0] = '\0';
				state.search_length = 0;
				state.search_forward = true;  // Forward search
			}
			else if (c == '?')
			{
				state.mode = SEARCH;
				state.search_buffer[0] = '\0';
				state.search_length = 0;
				state.search_forward = false;  // Backward search
			}
			else if (c == 'n')  // Next match
			{
				if (state.last_search_pattern[0] == '\0')
				{
					state.message = "No previous search pattern";
				}
				else
				{
					// Get current buffer position
					size_t current_pos = buffer_screen_to_index(buffer, state.cursor_y, state.cursor_x);
					ssize_t match_pos;
					
					if (state.last_search_forward)
					{
						// Search forward from current position + 1
						match_pos = buffer_find_pattern(buffer, state.last_search_pattern, current_pos + 1);
					}
					else
					{
						// Search backward from current position - 1
						match_pos = buffer_find_pattern_backward(buffer, state.last_search_pattern, current_pos - 1);
					}
					
					if (match_pos != -1)
					{
						buffer_index_to_screen(buffer, match_pos, &state.cursor_y, &state.cursor_x);
						state.message = "Pattern found";
					}
					else
					{
						state.message = "Pattern not found";
					}
				}
			}
			else if (c == 'N')  // Previous match (opposite direction)
			{
				if (state.last_search_pattern[0] == '\0')
				{
					state.message = "No previous search pattern";
				}
				else
				{
					// Get current buffer position
					size_t current_pos = buffer_screen_to_index(buffer, state.cursor_y, state.cursor_x);
					ssize_t match_pos;
					
					if (state.last_search_forward)
					{
						// Search backward (opposite of original forward search)
						match_pos = buffer_find_pattern_backward(buffer, state.last_search_pattern, current_pos - 1);
					}
					else
					{
						// Search forward (opposite of original backward search)
						match_pos = buffer_find_pattern(buffer, state.last_search_pattern, current_pos + 1);
					}
					
					if (match_pos != -1)
					{
						buffer_index_to_screen(buffer, match_pos, &state.cursor_y, &state.cursor_x);
						state.message = "Pattern found";
					}
					else
					{
						state.message = "Pattern not found";
					}
				}
			}
			else if (c == 'u')
			{
				undo_operation(state.undo_manager, buffer, &state);
			}
			else if (c == 18)
			{
				redo_operation(state.undo_manager, buffer, &state);
			}
			else if (c == ':')
			{
				state.mode = COMMAND;
				state.command_buffer[0] = '\0';
				state.command_length = 0;
			}
			else if (c == 'h')
			{
				if (state.cursor_x > 0)
				{
					state.cursor_x--;
				}
			}
			else if (c == 'l')
			{
				if (state.cursor_x < state.screen_cols - 1)
				{
					state.cursor_x++;
				}
			}
			else if (c == 'k')
			{
				if (state.cursor_y > 0)
				{
					state.cursor_y--;
				}
			}
			else if (c == 'j')
			{
				if (state.cursor_y < state.screen_rows - 1)
				{
					state.cursor_y++;
				}
			}
			else if (c == '0')
			{
				state.cursor_x = 0;
			}
			else if (c == '$')
			{
				size_t line_length = buffer_get_line_length(buffer, state.cursor_y);
				state.cursor_x = line_length;
			}
			else if (c == 'i')
			{
				state.mode = INSERT;

				state.undo_manager->in_insert_session = true;
				state.undo_manager->insert_start_x = state.cursor_x;
				state.undo_manager->insert_start_y = state.cursor_y;
				state.undo_manager->insert_start_pos = buffer->gap_start;
				state.undo_manager->current_insert_len = 0;

				state.undo_manager->current_insert_buffer[0] = '\0';
			}
		}
		else if (state.mode == INSERT)
		{
			// If ESC is clicked switch to NORMAL mode
			if (c == 27)
			{
				// Save INSERT operation before switching mode
				if (state.undo_manager->in_insert_session && state.undo_manager->current_insert_len > 0)
				{
					undo_push_operation(
						state.undo_manager,
						OP_INSERT,
						state.undo_manager->current_insert_buffer,
						state.undo_manager->insert_start_pos,
						state.undo_manager->insert_start_x,
						state.undo_manager->insert_start_y);
				}

				state.undo_manager->in_insert_session = false;
				state.mode = NORMAL;
			}
			// If BACKSPACE is clicked delete the character
			else if (c == 127)
			{
				buffer_delete_char(buffer);

				// Move cursor left
				if (state.cursor_x > 0)
				{
					state.cursor_x--;
				}
				else if (state.cursor_y > 0)
				{
					// At start of line, move to end of previous line
					state.cursor_y--;
					state.cursor_x = buffer_get_line_length(buffer, state.cursor_y);
				}

				if (state.undo_manager->in_insert_session && state.undo_manager->current_insert_len > 0)
				{
					state.undo_manager->current_insert_len--;
					state.undo_manager->current_insert_buffer[state.undo_manager->current_insert_len] = '\0';
				}
			}
			else if (c == 13 || c == 10)
			{
				buffer_insert_char(buffer, '\n');

				state.cursor_y++;
				state.cursor_x = 0;

				// Also track newline in insert buffer
				if (state.undo_manager->in_insert_session)
				{
					// Check if buffer needs to grow
					if (state.undo_manager->current_insert_len >= state.undo_manager->current_insert_capacity - 1)
					{
						state.undo_manager->current_insert_capacity *= 2;
						state.undo_manager->current_insert_buffer = realloc(
							state.undo_manager->current_insert_buffer,
							state.undo_manager->current_insert_capacity);
					}

					// Add newline
					state.undo_manager->current_insert_buffer[state.undo_manager->current_insert_len] = '\n';
					state.undo_manager->current_insert_len++;
					state.undo_manager->current_insert_buffer[state.undo_manager->current_insert_len] = '\0';
				}
			}

			// If Character is to be inserted insert the character
			else if (c >= 32 && c <= 126)
			{
				buffer_insert_char(buffer, c);

				state.cursor_x++;

				if (state.undo_manager->in_insert_session)
				{
					// Check if buffer needs to grow
					if (state.undo_manager->current_insert_len >= state.undo_manager->current_insert_capacity - 1)
					{

						// Double the capacity
						state.undo_manager->current_insert_capacity *= 2;
						state.undo_manager->current_insert_buffer = realloc(
							state.undo_manager->current_insert_buffer,
							state.undo_manager->current_insert_capacity);
					}

					// Add character
					state.undo_manager->current_insert_buffer[state.undo_manager->current_insert_len] = c;
					state.undo_manager->current_insert_len++;
					state.undo_manager->current_insert_buffer[state.undo_manager->current_insert_len] = '\0';
				}
			}
		}

		else if (state.mode == COMMAND)
		{
			if (c == 27)
			{
				state.mode = NORMAL;
				state.command_buffer[0] = '\0';
				state.command_length = 0;
				state.message = NULL;
			}

			else if (c == 127)
			{
				if (state.command_length > 0)
				{
					state.command_length--;

					// add null terminator at current_length position
					state.command_buffer[state.command_length] = '\0';
				}
			}

			else if (c == 13 || c == 10)
			{
				// Parse the command
				if (strcmp(state.command_buffer, "q") == 0 || strcmp(state.command_buffer, "quit") == 0)
				{
					break;
				}

				// Save the command
				else if (strcmp(state.command_buffer, "w") == 0 || strcmp(state.command_buffer, "write") == 0)
				{
					save_file(current_filename, buffer, &state);
				}

				// Save and quit
				else if (strcmp(state.command_buffer, "wq") == 0 || strcmp(state.command_buffer, "x") == 0)
				{
					save_file(current_filename, buffer, &state);
					break;
				}

				else if (state.command_buffer[0] == '\0')
				{
				}

				else
				{
					state.message = "This command is not recognized";
				}

				state.command_buffer[0] = '\0';
				state.command_length = 0;
				state.mode = NORMAL;
			}

			else if (c >= 32 && c <= 126)
			{
				if (state.message != NULL && state.message[0] != '\0')
				{
				}

				else if (state.command_length < 255)
				{
					state.command_buffer[state.command_length] = c;
					state.command_length++;
					state.command_buffer[state.command_length] = '\0';
				}

				else
				{
					state.command_buffer[0] = '\0';
					state.command_length = 0;

					state.message = "Command too long";
				}
			}
		}

		else if (state.mode == SEARCH)
		{
			if (c == 27) // ESC
			{
				state.mode = NORMAL;
				state.search_buffer[0] = '\0';
				state.search_length = 0;
				state.message = NULL;
			}
			else if (c == 127) // Backspace
			{
				if (state.search_length > 0)
				{
					state.search_length--;
					state.search_buffer[state.search_length] = '\0';
				}
			}
			else if (c == 13 || c == 10) // Enter
			{
				ssize_t match_pos;

				if (state.search_forward) 
				{
					match_pos = buffer_find_pattern(buffer, state.search_buffer, 0);
				}
				else
				{
					match_pos = buffer_find_pattern_backward(buffer, state.search_buffer, buffer->capacity - 1);
				}
				if (match_pos != -1)
				{
					buffer_index_to_screen(buffer, match_pos, &state.cursor_y, &state.cursor_x);
					state.message = "Pattern found";

					strcpy(state.last_search_pattern, state.search_buffer);
					state.last_search_forward = state.search_forward;
				}
				else
				{
					state.message = "Pattern not found";
				}

				state.search_buffer[0] = '\0';
				state.search_length = 0;
				state.mode = NORMAL;
			}
			else if (c >= 32 && c <= 126) // Printable characters
			{
				if (state.search_length < 255)
				{
					state.search_buffer[state.search_length] = c;
					state.search_length++;
					state.search_buffer[state.search_length] = '\0';
				}
			}
		}
	}

	scroll();

	scroll();
    
    // Free API key
    if (state.api_key != NULL)
    {
        free(state.api_key);
        state.api_key = NULL;
    }
}
