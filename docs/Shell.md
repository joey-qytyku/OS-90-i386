# OS/90 Built-in shell

The shell is called CMD.EXE and is a 32-bit program. It is responsible for executing programs from the command line and allows for some limited procedural programming.

CMD.EXE can detect DOS programs and message the virtual machine manager to execute it.

All commands are interpreted as a name with parameters and can nest. Kind of like lisp?

A token can be a value of a command.

## Syntax

### Variables and Constants

Variables are basically strings. They do not store any other data type. Math operators convert these strings into integers before operating on them.

CONST name 4

### Operators

The following operators work:
```
+
-
*
/
%
```

Inline asm is used to detect powers of two and use shift/and optimizations. popcnt would be useful, but it is not available for i386.

### if statement
```
IF = 1 1
ENDIF
```

### Procedures

```
PROC MyProcedure a1 a2 a3
    return + a1 a2 a3
ENDP
```
In C function form it would look like this:
```
PROC(MyProcedure(a1,a2,a3, return(+(a1 a2 a3))))
```

# Examples

```
VAR i

IF = i 3
    echo You guessed the right number!
ENDIF

```

# Internal Commands

# External Commands

## COMUTIL.EXE


