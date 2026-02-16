## Project: Vesper

## Project Overview
A terminal-based, modal text editor inspired by Vim/Neovim, built from scratch in C using pure termios (no ncurses). The goal is to implement a performant, minimal core editor first, then add advanced features like syntax highlighting, splits and AI-assisted inline suggestion.

## Progress

### Step 1 - Terminal Control (Raw Mode)
**Status:** Complete ✅

Successfully implemented low-level terminal control for raw mode input processing:

- ✅ Disabled canonical mode for character-by-character input
- ✅ Disabled echo to manually control character display
- ✅ Disabled signal keys (Ctrl-C, Ctrl-Z) for input capture
- ✅ Disabled input post-processing (Ctrl-S/Ctrl-Q, CR-to-NL)
- ✅ Safe terminal restoration on normal exit (`atexit()`)
- ✅ Terminal restoration on abnormal exit
- ✅ Terminal recovery after crashes verified

**What I Learned:**
- POSIX terminal control with `termios` API
- Managing terminal flags: `ICANON`, `ECHO`, `ISIG`, `IXON`, `ICRNL`
- Using `tcgetattr()` and `tcsetattr()` for terminal state management
- Cleanup handlers with `atexit()` for graceful exits

###  Step 2 — Main Editor Loop
**Status:** Complete ✅

Built the core event loop that drives the editor:

- ✅ Created infinite editor loop
- ✅ Reading exactly one keypress per iteration
- ✅ Implemented `EditorState` struct to track cursor and screen dimensions
- ✅ Clean exit on 'q' command
- ✅ Update state based on keypress (implementing cursor movement)
- ✅ Redraw screen every loop

**What I Learned:**
- ANSI escape sequences for terminal control (`\x1b[2J`, `\x1b[H`, `\x1b[ROW;COLUMNH`)
- Screen clearing and redrawing on every loop iteration
- Real-time visual feedback for cursor movement
- Terminal coordinate systems (1-based vs 0-based indexing)
- Proper update flow: clear → draw → position → read input → update state

### Step 3 — Text Buffer (Gap Buffer)
**Status:** Complete ✅

Implemented the gap buffer data structure for efficient text editing:

- ✅ Allocated initial buffer
- ✅ Defined gap_start and gap_end
- ✅ Mapped cursor to buffer index
- ✅ Inserted character at cursor
- ✅ Deleted character before cursor
- ✅ Moved cursor left/right
- ✅ Shifted gap when cursor moves
- ✅ Grew buffer when gap is full
- ✅ Verified no memory corruption

**What I Learned:**
- Gap buffer data structure fundamentals and why it's efficient for text editing
- Mapping logical cursor positions to physical array indices (accounting for the gap)
- Dynamic memory reallocation with `realloc()` and safe error handling
- Moving memory blocks with `memmove()` to shift text during buffer growth
- Character movement vs. index manipulation in gap shifting
- Building isolated test harnesses to verify data structure correctness before integration
- Visual debugging with custom print functions to inspect buffer state

**Current Functionality:**
The gap buffer fully supports insertion, deletion, cursor movement, and automatic growth. The buffer dynamically doubles in size when full, correctly preserving all text content. Comprehensive testing confirms no memory corruption across all operations. Ready to integrate with the main editor.

### Step 4 — Screen Rendering
**Status:** Complete ✅

Built a complete screen rendering system to display buffer content:

- ✅ Clear terminal reliably
- ✅ Render text rows
- ✅ Track cursor screen position
- ✅ Handle terminal resize
- ✅ Implement vertical scrolling
- ✅ Implement horizontal scrolling
- ✅ Draw status line

**What I Learned:**
- ANSI escape sequences for clearing (`\x1b[2J`) and cursor positioning (`\x1b[H`)
- Rendering gap buffer contents by skipping the gap region
- Iterating through buffer while avoiding gap indices (gap_start to gap_end-1)
- Separating rendering logic into its own module (render.c/render.h)
- Module dependencies: render.h needs to include buffer.h to use GapBuffer type
- Calculating cursor screen position by counting characters and newlines up to gap_start
- Using pointers to return multiple values from a function (row and col via `size_t *`)
- Using `ioctl()` with `TIOCGWINSZ` and `struct winsize` to dynamically get terminal dimensions
- Signal handling with `SIGWINCH` to detect and respond to terminal resize events
- Implementing viewport scrolling with row_offset and col_offset to show different portions of large files
- Rendering only visible portions of text for efficiency (checking both row and column ranges)
- Using ANSI escape codes for visual styling (`\x1b[7m` for inverted colors in status line)

**Current Functionality:**
Complete screen rendering system with viewport scrolling. Can display text from the gap buffer with proper vertical and horizontal scrolling when content exceeds terminal dimensions. Status line at bottom shows real-time cursor position. The editor correctly handles terminal resize events and adjusts the viewport accordingly. Text rendering is optimized to only draw visible content within the current viewport.

### Step 5 — Modal Editing (Vim-like)
**Status:** Complete ✅

Implemented Vim-style modal editing with NORMAL and INSERT modes:

- ✅ Define editor modes enum
- ✅ Implement NORMAL mode
- ✅ Implement INSERT mode
- ✅ Block text insertion in NORMAL
- ✅ Allow insertion in INSERT
- ✅ Mode switch via keypress
- ✅ Display current mode

**What I Learned:**
- Using C enums to represent distinct editor states (NORMAL vs INSERT)
- Mode-based input handling: same key triggers different actions depending on mode
- Restructuring input handling with nested conditionals (check mode first, then key)
- ASCII value 27 represents the ESC key for mode switching
- ASCII value 127 represents backspace on most systems
- Detecting printable characters with range check (32-126 ASCII)
- Integrating buffer operations (insert/delete) with user input
- Updating function signatures across multiple files to pass mode information
- Cross-module dependencies: render.h needed to include editor.h for EditorMode type

**Current Functionality:**
Full modal editing system. Editor starts in NORMAL mode where keys trigger commands (h/j/k/l for movement, q to quit, i to enter INSERT mode). In INSERT mode, all printable characters are inserted into the buffer, backspace deletes characters, and ESC returns to NORMAL mode. Status line displays current mode with visual distinction (inverted colors). Text can be typed, edited, and navigated seamlessly with mode-appropriate behavior.

### Step 6 — Navigation
**Status:** Complete 

Adding advanced navigation features for easier cursor movement:

- ✅ h/j/k/l movement (from Step 5)
- ✅ Arrow key movement
- ✅ Line start jump (0 or Home)
- ✅ Line end jump ($ or End)
- ✅ Page up/down
- ✅ Keep cursor in bounds

**What I Learned (so far):**
- Arrow keys send escape sequences: ESC + '[' + direction letter (A/B/C/D)
- Detecting multi-character input sequences by reading multiple times
- Distinguishing between ESC key alone vs ESC as part of a sequence
- Placing arrow key detection before mode checking so arrows work in both modes
- Using `continue` to skip rest of loop after handling arrow keys
- Reusing existing movement logic with boundary checks
- Home and End keys send different escape sequences depending on terminal (ESC[H or ESC[1~ for Home, ESC[F or ESC[4~ for End)
- Page Up/Down keys also use escape sequences (ESC[5~ and ESC[6~)
- Creating helper functions to encapsulate buffer operations: `buffer_get_line_length()` counts characters until newline, `buffer_get_total_lines()` counts newlines in entire buffer
- Calculating line boundaries by scanning buffer and counting characters/newlines
- Handling edge cases in page navigation: jumping to first/last line when out of bounds

**Current Functionality:**
Complete navigation system with multiple input methods. Arrow keys, h/j/k/l, and Page Up/Down all work for vertical/horizontal movement. Home/0 keys jump to line start, End/$ keys jump to line end. All navigation respects screen boundaries and works in both NORMAL and INSERT modes. Page navigation jumps by screen height with proper boundary checking.

### Step 7 — File Operations
**Status:** Complete ✅

Implemented full file I/O system for reading and writing files:

- ✅ Open file from CLI arg
- ✅ Read file into buffer
- ✅ Write buffer to disk
- ✅ Handle file errors safely

**What I Learned:**
- Command-line argument parsing with `argc` and `argv` in `main()`
- File I/O in C using `fopen()`, `fread()`, `fwrite()`, `fputc()`, and `fclose()`
- File modes: `"r"` for reading, `"w"` for writing (creates/overwrites file)
- Getting file size with `fseek()` and `ftell()`: seek to end, get position, seek back to start
- Dynamic memory allocation for file contents with `malloc()` and cleanup with `free()`
- Reading entire file at once vs. character-by-character
- Writing gap buffer to file by looping through buffer and skipping gap indices
- Detecting Ctrl+S keypress (ASCII value 19) for save command
- User feedback system with status messages in Vim style
- Adding message field to EditorState for displaying errors/success
- Updating function signatures across multiple files to pass state
- Handling file errors gracefully: file not found, permission denied, no filename
- Enter key detection (ASCII 13 or 10) to insert newlines in INSERT mode
- Different terminals send different codes for Enter (CR vs LF)

**Current Functionality:**
Complete file operations system. Can open files from command line (`./vesper file.txt`), read contents into gap buffer, edit text with full modal editing and navigation, save changes with Ctrl+S, and receive status feedback. If file doesn't exist on open, starts with empty buffer and creates file on save. Enter key inserts newlines for multi-line editing. Status line displays success ("File saved!") or error messages ("Error: No filename", "Error: Cannot write file") in Vim style.

### Step 8 — Command Mode
**Status:** Complete

Implementing Vim-style command mode for editor commands:

- ✅ Enter command mode
- ✅ Capture command string
- ✅ Parse command
- ✅ Implement quit
- ✅ Implement save
- ✅ Implement save + quit
- ✅ Display command errors

**What I Learned:**
- Adding new modes to the EditorMode enum (NORMAL, INSERT, COMMAND)
- Managing command input buffer with fixed-size array (256 chars)
- Tracking command length separately from buffer to know where to append
- Using `strcmp()` for string comparison in C (returns 0 when strings match)
- Supporting multiple command aliases (`:q` and `:quit`, `:w` and `:write`, `:wq` and `:x`)
- Parsing commands by checking string equality after user presses Enter
- Clearing command buffer and returning to NORMAL mode after execution
- Handling edge cases: empty commands, buffer overflow, unrecognized commands
- Setting appropriate error messages for user feedback ("This command is not recognized", "Command too long")
- Integrating command mode with existing save functionality
- Breaking out of main loop for quit commands vs continuing for save commands

**Current Functionality:**
Full Vim-style command mode implementation. Press `:` in NORMAL mode to enter COMMAND mode, type commands with visual feedback in status line, press Enter to execute or ESC to cancel. Supports `:q`/`:quit` to exit, `:w`/`:write` to save, `:wq`/`:x` to save and quit. Backspace removes characters from command, with 255 character limit. Unknown commands display error message. Command buffer clears after execution, returning to NORMAL mode.

### Step 9 — Undo/Redo
**Status:** Complete ✅

Implemented full undo/redo functionality with Vim-style action-level granularity:

- ✅ Decide undo granularity
- ✅ Record insert operations
- ✅ Record delete operations
- ✅ Implement undo stack
- ✅ Implement redo stack
- ✅ Restore cursor position

**What I Learned:**
- Action-level granularity: entire INSERT session is one undo operation
- Building undo/redo with dual stack architecture
- Tracking insert operations by recording characters as they're typed
- Handling newlines in insert buffer to preserve line structure
- Using linked list-backed dynamic arrays for unlimited undo history
- Preventing redo stack from being cleared during redo operations
- Restoring cursor position (screen coordinates) after undo/redo
- Managing undo manager lifecycle and initialization
- Difference between undo_push (clears redo) and undo_push_no_clear (preserves redo)

**Current Functionality:**
Full undo/redo system working. Users can press `u` in NORMAL mode to undo the last action (entire INSERT session or individual command). Press Ctrl+R to redo. Multiple undo/redo operations work correctly. Cursor position is restored to where it was when the action started. Newlines are properly tracked so multi-line inserts undo/redo correctly. New actions clear the redo stack as expected.

### Step 10 — Search
**Status:** Complete 

Implemented full search functionality with forward/backward search, navigation, and highlighting:

- ✅ Capture search query
- ✅ Find forward match
- ✅ Find backward match
- ✅ Jump to match
- ✅ Highlight matches

**What I Learned:**
- Adding SEARCH mode to editor mode enum
- Capturing search patterns in a dedicated buffer
- Implementing forward search from beginning of buffer
- Implementing backward search from end of buffer (reverse pattern matching)
- Matching patterns while skipping gap region in gap buffer
- Converting buffer index to screen coordinates (row, col) and vice versa
- Converting screen coordinates to buffer index for search positioning
- Tracking search direction (forward `/` vs backward `?`)
- Storing last search pattern and direction for `n` and `N` navigation
- Displaying search prompt in status line based on direction
- Implementing `n` key to repeat search in same direction
- Implementing `N` key to repeat search in opposite direction
- Modifying render function to accept search state parameters
- Real-time pattern matching during rendering for highlighting
- Using ANSI escape codes for text highlighting (`\x1b[43;30m` for yellow background with black text)
- Resetting colors after highlighting (`\x1b[0m`)
- Skipping ahead in render loop after printing highlighted match to avoid reprocessing
- Handling edge cases like pattern not found, empty patterns, and no previous search

**Current Functionality:**
Users can press `/` in NORMAL mode to search forward or `?` to search backward. Type the search pattern and press Enter to jump to the match. While typing in SEARCH mode, all matches are highlighted in yellow with black text. Press `n` to jump to the next match in the search direction, or `N` to jump in the opposite direction. The cursor moves to each occurrence. Status line shows the search prompt (`/pattern` or `?pattern`) as you type. Messages display "Pattern found" or "Pattern not found". Search is case-sensitive. Highlights only appear while actively typing in SEARCH mode and disappear when search completes or is canceled.

##  Challenges Encountered

### Step 1: Terminal Control
- **Echo Debugging:** When echo is disabled, debugging becomes difficult since you can't see what you're typing. Had to add temporary `printf()` statements to verify input was being captured.
- **Terminal State Corruption:** Initially forgot to restore terminal settings on abnormal exits, causing the terminal to remain in raw mode after crashes. Fixed by implementing `atexit()` cleanup.
- **Signal Handling:** Understanding that `atexit()` doesn't catch segfaults - the terminal restoration worked anyway due to shell cleanup, but learned about the limitations.

### Step 2: Main Editor Loop
- **Compilation Path Issues:** Had to navigate between working in the `src/` directory vs. root directory for compilation. Learned to use proper relative paths.
- **Silent Execution:** Initially thought the 'q' quit wasn't working because raw mode disables echo - the program was actually working correctly, just providing no visual feedback.
- **Header Organization:** Learning to properly separate function declarations (`.h`) from implementations (`.c`) and manage includes across multiple files.
- **State Management:** Had to learn about struct member access with the dot operator (`state.cursor_x`). Initially forgot the `state.` prefix when accessing cursor position variables.
- **Debug Timing:** Initially printed cursor coordinates before updating state, showing the previous position instead of current. Learned that order matters - read input → update state → display state.
- **Boundary Checks:** Implemented proper boundary checking to prevent cursor from moving outside screen dimensions (0 to cols-1, 0 to rows-1). This prevents negative coordinates or moving beyond terminal boundaries.
- **ANSI Escape Sequences:** Learning the syntax for terminal control codes:
  - `\x1b` is the ESC character (ASCII 27)
  - `\x1b[2J` clears the entire screen
  - `\x1b[H` moves cursor to home position (0,0)
  - `\x1b[ROW;COLUMNH` positions cursor at specific coordinates
- **Coordinate Systems:** Terminal uses 1-based coordinates (starting at 1,1) while internal state uses 0-based (starting at 0,0). Had to add +1 when converting: `cursor_y + 1, cursor_x + 1`
- **Parameter Order:** Initially reversed row/column order in cursor positioning. The format is `\x1b[ROW;COLUMNH` (y,x) not (x,y).
- **Screen Redraw Timing:** Learned to redraw at the beginning of the loop (before reading input) so the screen shows current state while waiting for the next keypress.

### Step 3: Gap Buffer Implementation
- **Syntax Errors:** Fixed multiple compilation issues including missing semicolons and missing `#endif` directive in header file.
- **Struct Field Naming:** Encountered inconsistency between using `size` vs `capacity` - resolved by standardizing on `capacity` throughout the codebase.
- **Index vs. Character Data:** Initially confused between manipulating indices and copying actual character data when implementing gap shifting. Learned that moving the gap requires copying characters from one side to the other, not just adjusting pointers.
- **Copy Direction in Gap Shift:** When moving cursor left vs. right, the direction of character copying is opposite:
  - Moving right: copy FROM `gap_end` TO `gap_start`, then increment both
  - Moving left: decrement both first, then copy FROM `gap_start` TO `gap_end`
  - Getting this backwards resulted in corrupted text
- **Linker Errors:** Learned to compile both `.c` files together (`gcc test.c buffer.c`) to resolve "undefined symbols" errors.
- **Format Specifiers:** Fixed warnings by using `%zu` instead of `%d` for `size_t` variables in printf statements.
- **Header Declarations:** Learned that every function implemented in `.c` must be declared in the corresponding `.h` file for external visibility.
- **Safe Memory Reallocation:** Initially assigned `realloc()` result directly to `buffer->data`, which would lose the original pointer if realloc failed. Learned to assign to a temporary variable first, check for NULL, then update the original pointer.
- **Buffer Growth Logic:** Had to carefully think through the math for moving text after reallocation:
  - `chars_to_move = old_capacity - old_gap_end` (text after the old gap)
  - `new_gap_end = new_capacity - chars_to_move` (where that text goes in the new buffer)
  - Order matters: calculate `chars_to_move` first, then use it to calculate `new_gap_end`
- **Variable Declaration Order:** Got compilation error using `chars_to_move` before declaring it - learned that variables must be declared before use.
- **Missing Header Includes:** Forgot to `#include <string.h>` for `memmove()`, resulting in implicit function declaration errors.
- **Testing Methodology:** Developed comprehensive testing strategy:
  - Created small buffers (capacity 5) to quickly trigger edge cases like buffer growth
  - Wrote `buffer_print_debug()` to visualize gap position and actual buffer contents with underscores showing the gap
  - Tested insertion, deletion, cursor movement, and growth in various combinations
  - Verified text integrity after every operation by inspecting actual memory contents
  - Confirmed expected vs. actual text output matches (e.g., "HelXlo!" after insertion in middle)
  - All tests passed with no garbage characters, proving no memory corruption

### Step 4: Screen Rendering
- **Header Dependencies:** Initially got "unknown type name 'GapBuffer'" error in render.h. Learned that when a header uses a type from another module, it must include that module's header (`#include "buffer.h"`).
- **Case Sensitivity in ANSI Codes:** Made typo with lowercase 'j' in `\x1b[2j` - ANSI escape sequences are case-sensitive and require uppercase `\x1b[2J` for screen clearing.
- **Logical Operators in C:** Initially used `AND` (word) instead of `&&` (C operator) when checking if index is in gap range.
- **Loop Logic for Gap Skipping:** Had to think carefully about the condition for skipping gap indices: `i >= gap_start && i < gap_end` (not <=, since gap_end is exclusive).
- **Returning Multiple Values:** Learned to use pointers as output parameters to return multiple values (row and col) from `render_get_cursor_pos()`. Required using `&` when calling and `*` when dereferencing inside the function.
- **Pointer Increment Syntax:** When incrementing a dereferenced pointer, parentheses are required: `(*row)++` not `*row++`, since `++` has higher precedence than `*`.
- **`ioctl()` and `struct winsize`:** Learned to use `ioctl(STDIN_FILENO, TIOCGWINSZ, &ws)` to query terminal size. The `struct winsize` members are `ws_row` and `ws_col` (not `rows`/`cols` or `ws_rows`/`ws_cols`) - compiler error messages were helpful in identifying correct names.
- **Type Mismatch with `size_t`:** `EditorState` had `screen_rows` and `screen_cols` as `int`, but `get_terminal_size()` used `size_t *`. Had to update the struct to use `size_t` for consistency and to eliminate compiler warnings.
- **Signal Handler Placement:** The `sigwinch_handler()` function must be defined BEFORE `editorLoop()` in `editor.c`, because the compiler needs to know about it when it's referenced in `signal(SIGWINCH, sigwinch_handler)`.
- **Function Prototype Mismatch:** Had `get_terminal_size()` declared without parameters in `terminal.h` but defined with `size_t *rows, size_t *cols` in `terminal.c`. Learned that declarations must exactly match the function signature.
- **Scrolling Logic:** Implementing viewport scrolling required careful thinking about offset calculations:
  - Vertical: When `cursor_y >= row_offset + screen_rows`, set `row_offset = cursor_y - screen_rows + 1`
  - Horizontal: When `cursor_x >= col_offset + screen_cols`, set `col_offset = cursor_x - screen_cols + 1`
  - Had to adjust scroll bounds when status line reduces available text area
- **Rendering Visible Range:** Initially rendered all text, then learned to check both row AND column ranges:
  - `if (current_row >= row_offset && current_row < row_offset + screen_rows && current_col >= col_offset && current_col < col_offset + screen_cols)`
  - Must track both `current_row` and `current_col` while iterating through buffer
- **Newline Handling:** When encountering `\n`, must increment `current_row` and reset `current_col` to 0. Initially placed the column increment in wrong location relative to newline check.
- **Status Line Integration:** Reserving bottom row for status line required:
  - Passing `screen_rows - 1` to `render_text()` instead of `screen_rows`
  - Adjusting scroll calculation to account for reduced text area
  - Positioning status line at `screen_rows` (bottom row) with inverted colors
- **Function Parameter Updates:** When adding col_offset and screen_cols parameters to `render_text()`, had to update:
  - Function signature in both render.h and render.c
  - All call sites in editor.c and test files
  - Missing parameters caused compilation errors

### Step 5: Modal Editing
- **Enum Syntax:** Initially used semicolons instead of commas to separate enum values. Learned that enum values are comma-separated (like a list), with the semicolon only at the very end after the type name.
- **Struct Field Semicolon:** Forgot semicolon after `EditorMode mode` field in struct. Every struct field needs a semicolon, even when using custom types like enums.
- **Input Handling Restructure:** Had to reorganize existing if-else chain for input handling by wrapping it in mode checks. This wasn't rewriting from scratch, just adding an outer `if (state.mode == NORMAL)` block around existing code and adding a new `else if (state.mode == INSERT)` block.
- **Missing Closing Brace:** Initially forgot the closing brace `}` for the NORMAL mode block before starting the INSERT mode block. This caused the `else if (state.mode == INSERT)` to be incorrectly nested.
- **Scroll Function Variable Names:** Had typos in scroll() function using `state.row_offset` instead of `state.col_offset` for horizontal scrolling checks, and used assignment operator `=` instead of subtraction `-` in offset calculation.
- **Buffer Function Parameters:** When calling `buffer_insert_char()`, initially forgot to pass the character parameter `c`. The function signature is `buffer_insert_char(buffer, c)` not just `buffer_insert_char(buffer)`.
- **Cross-Module Type Dependencies:** When updating `draw_status_line()` to accept `EditorMode` parameter, render.h needed to `#include "editor.h"` so it knew what the EditorMode type was. Learned about managing header dependencies across modules.
- **Printable Character Range:** Initially used `c < 126` instead of `c <= 126` when checking for printable characters. The tilde `~` character (ASCII 126) is printable and should be included in the range.
- **ASCII Values for Special Keys:** Learned that ESC is ASCII 27 and backspace is ASCII 127 (not 8 as initially thought). These are the values returned by `read()` when those keys are pressed.

### Step 6: Navigation (In Progress)
- **Escape Sequence Detection:** Learned that arrow keys don't send single characters but sequences of three bytes (ESC, '[', direction). Had to read multiple times after detecting ESC to determine if it's an arrow key or just ESC.
- **ESC vs Arrow Key Confusion:** Initially struggled with distinguishing between user pressing ESC key (mode switch) vs arrow key sequence (also starts with ESC). Solved by checking if ESC is followed by '[' - if not, it's just ESC key.
- **Placement of Arrow Detection:** Had to place arrow key handling before mode checking so arrows work in both NORMAL and INSERT modes. If placed inside mode blocks, would need duplicate code.
- **Continue Statement Usage:** Initially placed `scroll()` and `continue` in wrong locations, causing ESC to be processed twice. Learned that `continue` must be inside the arrow handling block to skip the rest of the loop iteration.
- **Boundary Checks:** Initially forgot to add boundary checks when copying movement logic, which could cause cursor to go out of bounds. Added same checks as h/j/k/l movement (checking cursor_x/y against screen dimensions).
- **Multiple Escape Sequences per Key:** Discovered that Home and End keys can send different sequences depending on terminal type. Had to handle both ESC[H and ESC[1~ for Home, and both ESC[F and ESC[4~ for End.
- **Line Length Calculation:** For jumping to end of line, needed to scan buffer to find where current line ends. Created `buffer_get_line_length()` helper function that iterates through buffer, skipping gap, counting characters until hitting newline.
- **Line Counting Logic:** Implemented `buffer_get_total_lines()` by iterating through entire buffer and counting newline characters. Started count at 1 since empty file has 1 line, then increment for each '\n'.
- **Page Navigation Boundaries:** Had to handle edge cases where Page Down would jump past end of file or Page Up would go negative. Used conditional logic to clamp cursor_y to valid range (0 to total_lines - 1).
- **Four-Character Sequences:** Page Up (ESC[5~) and Page Down (ESC[6~) are 4-character sequences, requiring an extra read() call after detecting '5' or '6' to verify the trailing '~'.
- **Missing Braces and Variables:** Ran into compilation errors from missing opening braces for else-if blocks and using undeclared variables (like `line_length` in wrong scope). Had to carefully match all braces and declare variables in correct scope.

### Step 7: File Operations
- **CLI Argument Parsing:** Learned how to accept command-line arguments in `main()` using `argc` and `argv`. `argc` counts arguments, `argv[0]` is program name, `argv[1]` is first argument (filename). Had to update function signatures to pass filename through the program.
- **File Size Calculation:** Used `fseek(fp, 0, SEEK_END)` to jump to end of file, `ftell(fp)` to get current position (equals file size), then `fseek(fp, 0, SEEK_SET)` to return to beginning. This pattern is necessary because C doesn't have a direct "get file size" function.
- **Memory Management for File Reading:** Allocated dynamic memory with `malloc(file_size + 1)` for the extra null terminator. Had to remember to check if `malloc()` returned NULL (allocation failed) before using the pointer. Also had to `free()` the memory after inserting characters into buffer to avoid memory leaks.
- **Writing Gap Buffer to File:** Couldn't just write the entire buffer array because of the gap. Had to loop through buffer indices, skip gap region (`i >= gap_start && i < gap_end`), and write only actual text characters. Used `fputc()` to write one character at a time.
- **Ctrl+S Detection:** Learned that Ctrl+S sends ASCII value 19 (not 's' or 'S'). Had to add detection before mode checks so save works in both NORMAL and INSERT modes. Initially forgot to pass updated state to save function.
- **Status Message System:** Added `char *message` field to EditorState for Vim-style status messages. Had to update `draw_status_line()` signature in both `.h` and `.c` files, plus update all call sites. Messages show in status bar: "File saved!" on success, error messages on failure.
- **Function Signature Updates:** When adding EditorState parameter to `save_file()`, had to pass `&state` (address of state) not just `state`, since function expects a pointer. Also had to update the call from `save_file(filename, buffer)` to `save_file(filename, buffer, &state)`.
- **Error Handling Strategy:** Decided to handle errors gracefully without crashing: file not found creates empty buffer, save without filename shows error message, permission denied shows error. Used early returns with status messages instead of crashing the program.
- **Enter Key Implementation:** Initially couldn't create new lines in files. Added detection for ASCII 13 and 10 (Enter key sends different codes on different terminals) to insert `'\n'` character. Had to add this check before the printable character range (32-126) check.
- **File vs Buffer State:** Had to think about when filename is NULL (started with `./vesper` no args) vs when filename exists. Save function needs to handle both cases - can't save without a filename, but that's not a fatal error.

### Step 8: Command Mode
- **Mode Management:** Added COMMAND as third mode to EditorMode enum. Had to carefully handle mode transitions: NORMAL → COMMAND (on `:` key), COMMAND → NORMAL (on ESC or Enter). Each mode now has distinct input handling.
- **String Comparison in C:** Learned that `==` doesn't work for string comparison in C - must use `strcmp()`. Function returns 0 when strings match, not 1 or true. Initially confused by this but makes sense once understood.
- **Command Buffer Management:** Used fixed-size array `char command_buffer[256]` with separate `command_length` counter. Had to manually manage null terminators - always set `command_buffer[command_length] = '\0'` after modifying. Forgot this initially and saw garbage characters.
- **Backspace in Command Mode:** When removing characters, must decrement `command_length` first, then add null terminator at new position. Can't just decrement - need to actually terminate the string or old characters remain visible.
- **Empty Command Handling:** User pressing Enter with empty command (just `:`) should do nothing, not show error. Added explicit check for empty string to handle this gracefully without cluttering with error messages.
- **Command Parsing Logic:** Used multiple `strcmp()` calls with `if-else` chain. Each command gets checked in order. Unknown commands fall through to final `else` block which sets error message. Considered switch statement but strings don't work with switch in C.
- **Supporting Command Aliases:** Implemented both short (`:q`, `:w`) and long (`:quit`, `:write`) versions like Vim. Also added `:x` as alias for `:wq`. Used OR operator in conditions: `strcmp(cmd, "q") == 0 || strcmp(cmd, "quit") == 0`.
- **Buffer Overflow Protection:** Must check `command_length < 255` before adding character to leave room for null terminator at position 255. Without this check, writing at position 256 would overflow the array (undefined behavior).
- **State Cleanup:** After executing command, must clear `command_buffer`, reset `command_length` to 0, and switch back to NORMAL mode. Forgetting any of these leaves editor in inconsistent state.
- **Integrating with Save Function:** Reused existing `save_file()` function for `:w` command. Had to pass correct parameters: `current_filename`, `buffer`, and `&state`. File save status messages automatically show in status line.

### Step 9: Undo/Redo
- **Newline Tracking:** Initially forgot to track newlines (`\n`) in the insert buffer when user pressed Enter. This caused redo to insert "firstsecond" instead of "first\nsecond" on separate lines. Fixed by adding newline character to current_insert_buffer when Enter is pressed.
- **Redo Stack Clearing:** When redoing an operation, pushing back to undo stack would clear the entire redo stack, preventing multiple redos. Fixed by creating separate `undo_push_operation_no_clear()` function that doesn't clear redo stack.
- **Function Declaration Order:** Got compiler error because `redo_push_operation()` was called before it was defined. Fixed by rearranging function order or adding forward declaration.
- **Cursor Position vs Buffer Position:** Screen cursor position (cursor_x, cursor_y) doesn't directly map to buffer index. Storing screen coordinates works because gap buffer always inserts at gap_start, which tracks with cursor movement.
- **Undo/Redo Logic:** Initially had redo_operation() using undo_stack instead of redo_stack, causing wrong behavior. Fixed by ensuring redo pops from redo_stack and pushes to undo_stack.

### Step 10: Search
- **Match Logic Bug:** Initially had `else { match_count = 0; }` inside the match success check, causing immediate reset after every matching character. This prevented multi-character patterns from ever matching. Fixed by moving the else block to only reset when characters don't match.
- **Backward Search Algorithm:** Needed to check if pattern ENDS at each position when searching backwards. Required calculating `index - (pattern_len - 1)` to find the start position of matches. Also had to ensure enough characters exist before the current position (`if (index < pattern_len - 1) continue;`).
- **Unsigned Loop with size_t:** Couldn't use `i >= 0` condition with size_t since it's unsigned and always true. Fixed by using `i > 0` and accessing `index = i - 1` within the loop body.
- **Status Line Parameter:** Had to pass `state.search_forward` (not `bool search_forward`) to `draw_status_line()`. The parameter is the variable value, not a type declaration.
- **Type Consistency:** Changed `cursor_x` and `cursor_y` from `int` to `size_t` to match function signatures, then had to update printf format specifiers from `%d` to `%zu`.
- **Screen to Buffer Index Conversion:** Needed helper function to convert screen coordinates (row, col) to buffer index for `n` and `N` navigation. Had to carefully iterate through buffer while skipping gap and counting newlines.
- **Variable Scope in Highlighting:** Initially declared `match_found` and `pattern_len` inside a nested block, making them out of scope for later use. Fixed by ensuring all variable declarations are at the correct scope level within the search mode check.
- **Brace Structure Errors:** Had extra closing braces that closed blocks too early, causing variables to go out of scope. Required careful attention to brace matching and indentation to fix.
- **Gap Handling in Pattern Matching:** When checking if pattern matches at a position, had to ensure no part of the match spans the gap region. If `check_pos` falls in gap (`check_pos >= gap_start && check_pos < gap_end`), the match is invalid.
- **Duplicate Function Call:** Accidentally had two `match_pos` declarations in SEARCH mode Enter handler, with second one overwriting the first. Removed duplicate to properly use forward/backward search results.

## Folder Structure
```
Vesper/
├── src/
│ ├── main.c
│ ├── editor.c
│ ├── editor.h
│ ├── terminal.c
│ ├── terminal.h
│ ├── buffer.c
│ ├── buffer.h
│ ├── render.c
│ ├── render.h
│ ├── input.c
│ ├── input.h
│ ├── commands.c
│ ├── commands.h
│ ├── utils.c
│ └── utils.h
├── include/
│ └── (optional headers if you prefer separate public API)
├── tests/
│ ├── insert_and_delete_char.c
│ ├── shift_cursor_test.c
│ ├── test_grow.c
│ ├── test_memory.c
│ ├── test_cursor_pos.c
│ ├── test_render_text.c
│ ├── test_vertical_scrolling.c
│ ├── test_horizontal_scrolling.c
│ └── terminal_tests.c
├── docs/
│ └── design_notes.md
├── .gitignore
├── Makefile
└── README.md
```

## Core Modules (What Each Folder/File Does)

### `src/main.c`

* Entry point
* Initializes terminal raw mode
* Creates editor state
* Runs the main loop
* Restores terminal on exit

### `src/terminal.*`

Handles terminal configuration and low-level I/O:

* raw mode setup
* key reading
* cursor positioning
* screen clearing
* dynamically gets terminal dimensions via `ioctl()`
* escape sequence parsing (arrow keys, etc.)

### `src/buffer.*`

Implements the **gap buffer** text structure:

* insertion with automatic growth
* deletion
* moving gap when cursor moves
* converting between cursor position and buffer index
* dynamic memory management

### `src/render.*`

Draws the screen:

* clears terminal reliably
* renders text from gap buffer with viewport scrolling
* tracks cursor screen position
* draws status line at bottom with cursor info
* implements vertical and horizontal scrolling
* optimizes rendering by only drawing visible content

### `src/editor.*`

Manages the editor state and main loop:

* holds `EditorState` with cursor, screen dimensions, and viewport offsets
* handles `SIGWINCH` for terminal resize
* implements `scroll()` function for viewport management
* processes keypresses and updates state
* drives the main editor loop

### `src/input.*`

Handles key events:

* normal vs insert mode
* mapping keys to actions
* command mode input (later)

### `src/commands.*`

Parses and executes `:` commands:

* `:w` (save)
* `:q` (quit)
* `:wq` (save + quit)
* `:help` (optional)

### `src/utils.*`

Helper functions:

* string utilities
* file utilities
* error handling

---

## Development Roadmap (First Milestones)

### **Milestone 1: Core Engine**

* ✅ Terminal raw mode + main loop
* ✅ Gap buffer (complete - all functionality implemented and tested)
* ✅ Screen rendering (complete - all 7 items done)
* ✅ Basic typing and backspace
* ✅ Full screen redraw with text from buffer

### **Milestone 2: Modal Editing**

* ✅ Normal mode
* ✅ Insert mode
* ✅ Escape to normal
* ✅ Basic navigation commands
* ✅ Advanced navigation (arrow keys complete, more features in progress)

### **Milestone 3: File I/O**

* ✅ Open file
* ✅ Save file
* ✅ Command mode `:w`, `:q`, `:wq`

### **Milestone 4: Advanced Features (optional)**

✅ Undo/redo
✅ Search (`/pattern`)
* Syntax highlighting
* Split windows/tabs

### **Milestone 5: AI Assistant (later)**

* Inline suggestions
* Ghost text rendering
* Accept/cancel
