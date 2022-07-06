/**
 * @file Printer.c
 * @author Joey Qytyku
 * @brief  Parallel port driver
 * @date 2022-06-29
 * @copyright Copyright (c) 2022
 */

#include <DriverLib/Drivers.h>
#include <Platform/Resource.h>

char *name = "LinePrinter";

DriverParamBlock line_printer
{
    .name = &name,
    .flags = DRVINF_INT
}

void InitLPT()
{
    RequestFixedLines(1<<7, Handler, name);
}
