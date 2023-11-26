#include <stdio.h>
#include <string.h>
#include "queue.h"

const char *get_enum_name(int enum_value);
int **map_processes_to_statistics(int processes_amount, Process *processes);
void show_processes_statistics(int processes_amount, int **processes);