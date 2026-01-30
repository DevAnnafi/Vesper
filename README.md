## Project: Vesper

## Project Overview
A terminal-based, modal text editor inspired by Vim/Neovim, built from scratch in C using pure termios(no ncurses). The goal is to implement a performant, minimal core editor first, then add advanced features like syntax highlighting, splits and AI-assisted inline suggesstion.

## Progress

### Step 1 - Terminal Control (Raw Mode)
**Status:** Complete

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
**Status:** In Progress (5/6 complete)

Building the core event loop that drives the editor:

- ✅ Created infinite editor loop
- ✅ Reading exactly one keypress per iteration
- ✅ Implemented `EditorState` struct to track cursor and screen dimensions
- ✅ Clean exit on 'q' command
- ✅ Update state based on keypress (implementing cursor movement)
- 🔲 Redraw screen every loop

**Current Functionality:**
The editor enters raw mode, processes keypresses in a loop, and exits cleanly when 'q' is pressed. Working on implementing h/j/k/l cursor movement and screen rendering next.

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


