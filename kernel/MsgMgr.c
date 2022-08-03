/*
     This file is part of OS/90.

    OS/90 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

    OS/90 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with OS/90. If not, see <https://www.gnu.org/licenses/>. 
*/

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
