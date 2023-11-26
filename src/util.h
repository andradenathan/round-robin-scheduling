#include <stdio.h>
#include <string.h>
#include "queue.h"

#define PROCESS_AMOUNT 10
#define START_LIMIT 10
#define DURATION_LIMIT 20

#define IO_NOT_DONE -1
#define IO_IN_PROGRESS 0
#define IO_DONE 1

#define IO_LIMIT 3
#define IO_TYPES_AMOUNT 3
#define IO_DISK_DURATION 3
#define IO_TAPE_DURATION 5
#define IO_PRINTER_DURATION 10

const char *get_enum_name(int enum_value);
int **map_processes_to_statistics(int processes_amount, Process *processes);
void show_processes_statistics(int processes_amount, int **processes);