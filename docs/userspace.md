# Executables

PE/COFF files are used to run programs. They use the flat model and sections are loaded wherever desired by the executable as long as it is below the 3G range.

mili2020

# System Call Interface

INT 31H is the single system call vector. Returns values are 32-bit and match with the kernel return values: zero for no error, -1 for error, greater than zero for other status or error.
