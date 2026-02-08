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

* Undo/redo
* Search (`/pattern`)
* Syntax highlighting
* Split windows/tabs

### **Milestone 5: AI Assistant (later)**

* Inline suggestions
* Ghost text rendering
* Accept/cancel
