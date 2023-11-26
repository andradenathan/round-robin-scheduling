#include "util.h"
#include <stdarg.h>

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

void highlighted_print(Colors color, const char *string, ...)
{
    const char *color_codes[] = {
        "\033[0;30m", // BLACK
        "\033[0;31m", // RED
        "\033[0;32m", // GREEN
        "\033[0;33m", // YELLOW
        "\033[0;34m", // BLUE
        "\033[0;35m", // MAGENTA
        "\033[0;36m", // CYAN
        "\033[0;37m"  // WHITE
    };

    const char *reset_code = "\033[0m";

    if (color >= BLACK && color <= WHITE)
    {
        printf("%s", color_codes[color]);
    }

    va_list args;
    va_start(args, string);
    vprintf(string, args);
    va_end(args);

    // Reseta a formatação
    printf("%s", reset_code);
}