#include <Type.h>

typedef struct {
    word    in_command;
    word    out_status;
    pvoid   data;
}Message,*PMessage;

typedef struct
{
    byte    name[8];
    byte    current_command;
    byte    max_commands;
    PMessage msg_buff;
}Mbx,*PMbx;
