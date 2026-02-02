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
**Status:** In Progress (2/7 complete)

Building the screen rendering system to display buffer content:

- ✅ Clear terminal reliably
- ✅ Render text rows
- ⏳ Track cursor screen position
- ⏳ Handle terminal resize
- ⏳ Implement vertical scrolling
- ⏳ Implement horizontal scrolling
- ⏳ Draw status line

**What I Learned:**
- ANSI escape sequences for clearing (`\x1b[2J`) and cursor positioning (`\x1b[H`)
- Rendering gap buffer contents by skipping the gap region
- Iterating through buffer while avoiding gap indices (gap_start to gap_end-1)
- Separating rendering logic into its own module (render.c/render.h)
- Module dependencies: render.h needs to include buffer.h to use GapBuffer type

**Current Functionality:**
Can clear the screen reliably and render text from the gap buffer, correctly skipping over the gap. Text displays properly even after buffer modifications like insertions in the middle.

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
- **Variable Declaration Order:** Got compilation error when using `chars_to_move` before declaring it - learned that variables must be declared before use.
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
│ ├── test_render_text.c
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

* clears terminal
* renders text from buffer
* prints visible lines
* status bar (coming soon)
* cursor placement (coming soon)
* handles scrolling offsets (coming soon)

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
* 🔄 Screen rendering (2/7 complete)
* 🔄 Basic cursor movement (gap buffer supports it, need to integrate with editor)
* ⏳ Basic typing and backspace
* ⏳ Full screen redraw with text from buffer

### **Milestone 2: Modal Editing**

* Normal mode
* Insert mode
* Escape to normal
* Basic navigation commands

### **Milestone 3: File I/O**

* Open file
* Save file
* Command mode `:w`, `:q`, `:wq`

### **Milestone 4: Advanced Features (optional)**

* Undo/redo
* Search (`/pattern`)
* Syntax highlighting
* Split windows/tabs

### **Milestone 5: AI Assistant (later)**

* Inline suggestions
* Ghost text rendering
* Accept/cancel