## Project: Vesper

## Project Overview
A terminal-based, modal text editor inspired by Vim/Neovim, built from scratch in C using pure termios(no ncurses). The goal is to implement a performant, minimal core editor first, then add advanced features like syntax highlighting, splits and AI-assisted inline suggesstion.

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

Building the core event loop that drives the editor:

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
**Status:** In Progress (8/10 complete)

Implementing the gap buffer data structure for efficient text editing:

- ✅ Allocated initial buffer
- ✅ Defined gap_start and gap_end
- ✅ Mapped cursor to buffer index
- ✅ Inserted character at cursor
- ✅ Deleted character before cursor
- ✅ Moved cursor left/right
- ✅ Shifted gap when cursor moves
- ⏳ Grow buffer when gap is full
- ⏳ Verify no memory corruption

**What I Learned:**
- Gap buffer data structure fundamentals
- Mapping logical cursor positions to physical array indices
- Character movement vs. index manipulation in gap shifting
- Boundary checking to prevent buffer underflow/overflow
- Testing buffer operations in isolation before integration

**Current Functionality:**
The gap buffer can insert and delete characters, and the cursor can move left and right with the gap following it correctly. Next step is implementing dynamic buffer growth when the gap fills up.

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
  - Moving right: copy FROM `gap_end` TO `gap_start`
  - Moving left: copy FROM `gap_start` TO `gap_end`
- **Linker Errors:** Learned to compile both `.c` files together (`gcc test.c buffer.c`) to resolve "undefined symbols" errors.
- **Format Specifiers:** Fixed warnings by using `%zu` instead of `%d` for `size_t` variables in printf statements.
- **Header Declarations:** Learned that every function implemented in `.c` must be declared in the corresponding `.h` file for external visibility.
- **Testing Methodology:** Developed simple test harnesses to verify buffer operations work correctly before integrating with the main editor.

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
│ ├── buffer_tests.c
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

* insertion
* deletion
* moving gap
* converting between cursor position and buffer index

### `src/render.*`

Draws the screen:

* prints visible lines
* status bar
* cursor placement
* handles scrolling offsets

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

## 🧩 Development Roadmap (First Milestones)

### **Milestone 1: Core Engine**

* ✅ Terminal raw mode + main loop
* Gap buffer
* Basic cursor movement
* Basic typing and backspace
* Full screen redraw

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


