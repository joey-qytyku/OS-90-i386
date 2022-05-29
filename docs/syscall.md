# System Calls

System calls are performed using the far call instructions through a call gate. They can be called from drivers and user programs. This is not to be confused with the driver API, which provides a different set of functions for a different purpose.

Should I really do this?

# GDT

The GDT entry number 7 is a call gate.
