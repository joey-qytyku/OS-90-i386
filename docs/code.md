# Code Rules

The entire source code of the OS/90 kernel and associated drivers follow these guidelines. I recommend that programs written for the OS e written in this style as well, but there is no requirement and pascal case should be compatilble in practice.

# C Guidelines

Pascal case:
* Functions
* Type definitions
* Typedef of another type (Handle => dword)

Underscore case:
* Function local variables
* Static variables
* Struct fields

No caps:
* Integral typedefs (byte, word, dword, ...)

Global:
* functions must never be inlined.
* variables should be avoided unless there is no better way.

All caps:
* Macro definitions (no function-like macros)

Avoid using unions. Never use pre-inc/dec.

# Assembly Guidelines

x86 assembly code must be compatible with the Netwide Assembler.
* Tabs should be used between instructions and operands.
  * Tab size is 12
* No spaces after the comma
* Cases same as C
* No instructions after a label

Local variables are not really a concern here.

Register parameters can be passed in any way that is convenient, as long as it is described in comments.
