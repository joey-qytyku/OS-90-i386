The GUI library is called QI for Quadrant Interface. It is written in C.

## API Types

### Window Type

```c
enum {
	Dialogue, // Has an X
	Task // Has min/max, X
};
```

## API functions

Quadrant zero is the starting one

## `Stat GuiEnable(void);`

## `Stat GuiDisable(void);`

## `Stat CreateMWin(MasterWin*, Type t);`

Returns NULL if failed. The master window is created and appears on the screen.

## `Stat SetInst(WinInst*)`

Allows for switching interfaces.

## DestroyWin(WinInst*);

Quad SplitVertical(Quad q);
Quad SplitHorizontal(Quad q);
Stat AppendObject(Quad q, Object *o);

