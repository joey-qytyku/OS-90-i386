# Code Rules

The entire source code of the OS/90 kernel and associated drivers follow these guidelines. 


# C Code Guidelines

Pascal case:
* Functions
* Type definitions

Underscore case:
* Function local variables
* Static variables
* Struct fields

Variables and non-inline global functions should not be named exactly after x86 instructions.

Global functions must never be inlined.

Global variables should be avoided unless there is no better way.

All caps:
* Macro definitions (no function-like macros)

Avoid using unions. Never use pre-inc/dec.

# Assembly Guidelines

x86 assembly code must be compatible with the Netwide Assembler.
* Tabs should be used between instructions and operands.
  * Tab size is 12
* No spaces after the comma
* Rest same as C

Local variables are not really a concern here.
