# This Document

The goal is to establish how the source code is written. Some of these are just good practice while other rules are style and personal prefference.

It is recommended that programs written for OS/90 are written in accordance with at least the style guidelines.

# Code Rules

The entire source code of the OS/90 kernel and associated drivers follow these guidelines.

# C Guidelines

## Style Rules

These rules decide how the code should be styled and have no affect of functionality.

### Brackets

Always use wide open curly brackets. It is easier to read.

```
if/function/whatever {
    // Cringe
}

if/function/whatever
{
    // Correct
}
```

For structures, its okay since they don't have much nesting.

### Capitalization and Naming

* Function names are pascal-cased and prefixed with their subsystem
* Variable names are snake-cased
* Type names are all-caps aka macro case
### Code Formating

* Indents are four spaces
* Line endings are always CRLF for DOS/Windows compatibility
* Struct members and long parameter lists should be aligned with spaces
* Lines should be no longer than 80 characters for viewing in DOS

### Code Conventions
* Pointer arguments used to convey return or parameter values should be prefixed with IN or OUT.
* Extern globals in header files should be prefixed with _

I have determined that functions with long parameter lists are most readable when formatted as:
```
return-type FunctionName(
    type    param,
    type    param
){
        // Code
}
```

## Programming Rules

The following are rules for how OS/90 software written in C should operate and may affect how code is generated by the compiler.

The target platform demands high performance and minimum memory usage.

Never:
* Use pre-increment or decrement
* Return structs (It's bad for performance)

It is preffered that struct passed to a function is the only parameter. Structures should be as self contained as possible and be used for a certain task or type of information.

```c++
typedef struct { int i; ... }EXAMPLE,*PEXAMPLE;

void Good(PEXAMPLE j);
void Bad(int j, PExample_t );
```

## Comments

Single line comments only except if they do not work (macro comments?) Licenses should be multi-lined for clarity

## Function Naming

A function with external linkage must specify which subsystem it is part of:
|Prefix|Subsystem|
-|-
Ke | Nothing specific
In | Interrupts
Sc | Scheduler
Me | Memory manager
Pn | Resource manager/Plug-and-play
Ia | IA32 and IBM PC related functions
Sy | System call
Fs | Filesystem

## Global Variables

Global variables are acceptable for improving performance, as long as they are accessed using static inline functions or macros in the same header file that externs them. An underscore prefix should be used to indicate that they are abstracted by other functions in the header. Otherwise, globals should be avoided.

## Structures

Structures of arrays should be prefered in most cases unless the reverse is more acceptable. This is to improve cache locality and reduce memory usage.

## Parameters

Double pointers are tolerable and are somtimes preffered. If a function needs to access a pointer

Array parameters should never be used (eg. param[x]) because it does nothing useful. Arrays, however, can be passed to functions by value in place of a pointer.

As previously mentioned, the IN and OUT macros should be used to indicate what pointer parameters are for.

## Types

General purpose strings are of the type IMUSTR and pointers to them are PIMUSTR. Semantically, IMUSTR is a `const char * const` and the size is the width of a pointer.

Typedefs should never be made for arrays.

# Optimization Guidelines

Some are general concepts, while other tips here are specific to OS/90.

* Optimize the code that runs repeadedly and is the slowest, aka speed critical
* Do not optimize code that runs only once or takes the least execution time
* Always be efficient AND elegant
* Avoid premature optimization
* Slow code is always bad code
* Use a structure of arrays when possible
* Pack structures and use bit fields to save memory
* Align structures for performance

## Userspace

Userspace is generally larger than kernel space and demands more resources.

* Allocate memory using the user API rather than DPMI or XMS
* Avoid accessing virtual devices to reduce arbitration overhead
* Unfreeze blocks if less frequently used
* Do not unfreeze blocks for performance 
* Release blocks to reuse allocated memory
* Use API data structures

Memory management uses handles which can be locked or released. Locking memory will return a pointer and ensure it does not move. Unlocking memory means that the block can be relocated to reduce fragmentation.

Unlocking can be used with the intention of locking it again and reusing the block.

# Assembly Guidelines

x86 assembly code must be compatible with the Netwide Assembler.
* Tabs should be used between instructions and operands.
  * Tab size is 8 spaces
* No spaces after the comma
* Cases same as C
* No instructions after a label

Local variables are not really a concern here.

Register parameters can be passed in any way that is convenient, as long as it is described in comments.

Variables should use Hungarian case because assembly is typeless, but this is not a huge deal because assembler code is rare in OS/90 and is for things that are hard to do in C.

```
i{any type}Name - Signed
wName - Word
bName - Byte
lName - Long, aka dwordsw
qName - Quad word
sName - String of bytes
wfpName 16-bit far pointer (SEG:OFF16)
lfpName 32-bit far pointer (SEG:OFF32)
{j,f}Name   Jump/call target or function pointer
p{w,b,l,q,s}Name pointer to {x}, eg. pwName
a{w,b,l,q,s}Name array of {x}
```
Examples:
```
pawVgaTextMem = 0B8000h
asArgv
ilArgc
```

As seen in some of the examples, the type info may be too verbose, so comments should be prefered if it's too long.
