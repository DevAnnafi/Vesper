#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "terminal.h"
#include "editor.h"
#include "render.h"
#include "buffer.h"

EditorState state;

void sigwinch_handler(int sig)
{
    get_terminal_size(&state.screen_rows, &state.screen_cols);
    screen_clear();
    fflush(stdout);
}

void scroll()
{
    if (state.cursor_y >= state.row_offset + state.screen_rows)
    {
        state.row_offset = state.cursor_y - state.screen_rows + 1;
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

int main()
{
    enableRawMode();
    
    // Initialize state
    state.row_offset = 0;
    state.col_offset = 0;
    state.cursor_x = 0;
    state.cursor_y = 0;
    get_terminal_size(&state.screen_rows, &state.screen_cols);
    signal(SIGWINCH, sigwinch_handler);

    GapBuffer *buffer = buffer_create(2048);

    // Add test lines with LONG content (to test horizontal scrolling)
    for (int i = 0; i < 20; i++) {
        char line[200];
        sprintf(line, "Line %d: This is a very long line that extends beyond the terminal width to test horizontal scrolling functionality properly\n", i);
        for (int j = 0; line[j] != '\0'; j++) {
            buffer_insert_char(buffer, line[j]);
        }
    }

    while (1)
    {
        printf("\x1b[2J");
        printf("\x1b[H");

        render_text(buffer, state.row_offset, state.screen_rows, state.col_offset, state.screen_cols);

        printf("\x1b[%d;%dH", state.cursor_y + 1, state.cursor_x + 1);

        fflush(stdout);

        char c;
        read(STDIN_FILENO, &c, 1);

        if (c == 'q') 
        {
            break;
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
            state.cursor_x++;  // Allow moving right without limit to test scrolling
        }
        else if (c == 'j') 
        {
            state.cursor_y++;
        }
        else if (c == 'k') 
        {
            if(state.cursor_y > 0) 
            {
                state.cursor_y--;
            }
        }

        scroll();
    }
    
    disableRawMode();
    return 0;
}
