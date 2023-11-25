#include "util.h"

const char *get_enum_name(int enum_value)
{
    switch (enum_value)
    {
    case IO_DISK:
        return "Disco";
    case IO_TAPE:
        return "Fita magnética";
    case IO_PRINTER:
        return "Impressora";
    default:
        return NULL;
    }
}