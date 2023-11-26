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

int **map_processes_to_statistics(int processes_amount, Process *processes)
{
    int **processes_statistics = (int **)malloc(processes_amount * sizeof(int *));

    for (int i = 0; i < processes_amount; i++)
    {

        processes_statistics[i] = (int *)malloc(4 * sizeof(int));
    }

    for (int j = 0; j < processes_amount; j++)
    {
        processes_statistics[j][0] = processes[j].pid;
        processes_statistics[j][1] = processes[j].enqueued_time;
        processes_statistics[j][2] = processes[j].dequeued_time;
        processes_statistics[j][3] =
            processes[j].dequeued_time - processes[j].enqueued_time;
    }

    return processes_statistics;
}

void show_processes_statistics(int processes_amount, int **processes)
{
    const char *label[] = {
        "Processos",
        "PID",
        "Exec.",
        "Final.",
        "Dur."};

    for (int i = 0; i < 5; ++i)
    {
        printf("%13s ", label[i]);
    }
    printf("\n");
    for (int i = 0; i < processes_amount; ++i)
    {
        printf("%-10s ", " ");

        for (int j = 0; j < 4; ++j)
        {

            printf("%14d ", processes[i][j]);
        }

        printf("\n");
    }

    printf("----------------\n");
}