## Project: Vesper

## Project Overview
A terminal-based, modal text editor inspired by Vim/Neovim, built from scratch in C using pure termios(no ncurses). The goal is to implement a performant, minimal core editor first, then add advanced features like syntax highlighting, splits and AI-assisted inline suggesstion.

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

* Terminal raw mode + main loop
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


