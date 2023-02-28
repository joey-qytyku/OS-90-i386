# Overview

The userspace specification details the way that programs and the user interface with the system. It is highly recommended that programs follow the userspace protocols.

The goal is to ensure that their are no competing standards, and that OS/90 can avoid being fragmented and cluttered, and instead be an integrated experience.

OS/90 is intended to be a desktop operating system for only one user.

# Role of DPMI

Userspace applications are PE/COFF files with special executable headers that use DPMI to load the executable data and also handle dynamic linking. The loading code is called by interrupt Each process is a virtual machine, given the impression of being a single-tasking DOS machine with direct access to hardware. Memory is shared by all processes using the same LDT.

# INIT program

INIT.EXE is the program that initializes the system. It must be 32-bit.

# Directory Structure

```
OS90/
    APPS/
        (App name)/
            (name).ICO
            (name).PIF
    SYSTEM/
        INIT/
            STARTUP.CMD
    BIN/
        CMD.EXE
    LIB/
    USER/
        DESKTOP/
        RECYCLE/
        DOCS/
```

# INIT System

OS/90 has a basic INIT system. Services are stored in SYSTEM/INIT and have a RUN.CMD script.

# Applications Folder

Installable programs are placed in the APPS directory. This is somewhat like macOS, which has an Application folder. Programs can be deleted by simply using DELTREE and removing the folder. Installation is as simple as copying to the APPS directory.

An application can contain anything in this folder, as long as the executable and PIF file are present and have the same name as the program (with different extensions).

The shell is able to run programs in the APPS folder.

## PIF

PIF files work differently than they do in Windows 3.x. Direct hardware access is never allowed for any programs, unlike in Windows (old versions). Memory requirements are deduced by the kernel based on the executable header (That is why PE does not require PIF files).

The format is plain text:
```
Name=Foo Bar
Version=1.0
Exec=foobar.exe
```

Exec specifies the file that should be executed when the program runs. This must be in the root of the application.

# Display Protocol

For the display to work, there are a few components that must be present. When the kernel passes control to the INIT program, there is no display driver loaded. Instead, the kernel outputs text using INT 10H.

## Display Drivers

Display drivers are regular OS/90 drivers that use the event system to implement display drawing. 2D accelerations are supported. 3D will not be.

A DD must support pixel plotting, getting the dimensions, and managing buffers. Other features do not need to be present.

## Window Manager

The WM must implement the entire GUI API and can be any program. The WM is executed by the INIT program
