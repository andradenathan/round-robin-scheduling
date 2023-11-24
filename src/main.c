#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include "queue.h"

#define PROCESS_MAX_AMOUNT 10
#define START_TIME_LIMIT 10
#define DURATION_TIME_LIMIT 20
#define IO_MAX_AMOUNT 3
#define IO_TYPES_AMOUNT 3

#define IO_DISK_DURATION 3
#define IO_TAPE_DURATION 5
#define IO_PRINTER_DURATION 10

int device_current_time;
int ended_processes;
Process processes[PROCESS_MAX_AMOUNT];
Process *running;
Queue *high, *low;

Queue *io[IO_TYPES_AMOUNT];
Process *io_running[IO_TYPES_AMOUNT];
int io_duration[IO_TYPES_AMOUNT] = {IO_DISK_DURATION, IO_TAPE_DURATION, IO_PRINTER_DURATION};
int io_progress[IO_TYPES_AMOUNT];

FILE *log_file;

void generate_processes(void)
{
    fprintf(log_file, "--- Processos gerados aleatoriamente: ---\n");
    for (int i = 0; i < PROCESS_MAX_AMOUNT; i++)
    {
        Process *p = &processes[i];
        p->pid = i;
        p->start = rand() % START_TIME_LIMIT;
        p->duration = (rand() % DURATION_TIME_LIMIT) + 1;
        p->progress = 0;

        if (p->duration > 1)
        {
            p->io_amount = rand() % (IO_MAX_AMOUNT + 1);
            p->io_starts = malloc(p->io_amount * sizeof(int));
            p->io_types = malloc(p->io_amount * sizeof(int));
            p->io_done = malloc(p->io_amount * sizeof(int));

            for (int j = 0; j < p->io_amount; j++)
            {
                p->io_starts[j] = (rand() % p->duration) + 1;
                p->io_types[j] = rand() % IO_TYPES_AMOUNT;
                p->io_done[j] = -1;
            }
        }
        else
        {
            p->io_amount = 0;
        }

        fprintf(log_file, "PID %d\n", p->pid);
        fprintf(log_file, "Start %d\n", p->start);
        fprintf(log_file, "Duration %d\n", p->duration);
        fprintf(log_file, "IO amount %d\n", p->io_amount);
        for (int j = 0; j < p->io_amount; j++)
        {
            fprintf(log_file, "Type: %d Start: %d\n", p->io_types[j], p->io_starts[j]);
        }
        fprintf(log_file, "==============\n");
    }
}

void tick(void)
{
    /* Coloca processos novos na fila */
    for (int i = 0; i < PROCESS_MAX_AMOUNT; i++)
    {
        Process *p = &processes[i];
        if (p->start == device_current_time)
            queue_add(high, p);
    }

    /* Move processos interrompidos para a fila de baixa prioridade */
    if (running)
    {
        if (running->progress == running->duration)
        {
            ended_processes;
            running->tick_end = device_current_time;
            running = NULL;
        }
        else
        {
            queue_add(low, running);
        }
    }

    do
    {
        /* Escolhe o processo que será executado */
        if (!queue_is_empty(high))
        {
            running = queue_remove(high);
        }
        else if (!queue_is_empty(low))
        {
            running = queue_remove(low);
        }

        if (!running)
        {
            break;
        }
        else if (!running->progress)
        {
            running->tick_start = device_current_time;
        }

        /* Verifica se o processo que foi escolhido quer fazer IO */
        for (int i = 0; i < running->io_amount; i++)
        {
            int start = running->io_starts[i];

            if (start == running->progress && running->io_done[i] == -1)
            {
                int type = running->io_types[i];
                running->io_done[i] = 0;
                queue_add(io[type], running);
                running = NULL;
                break;
            }
        }
    } while (!running);

    /* Escolhe os processos que farão IO */
    for (int i = 0; i < IO_TYPES_AMOUNT; i++)
    {
        if (io_progress[i] == io_duration[i])
        {
            Process *finished = io_running[i];

            switch (i)
            {
            case IO_DISK:
                queue_add(low, finished);
                break;
            case IO_TAPE:
            case IO_PRINTER:
                queue_add(high, finished);
                break;
            }

            for (int j = 0; j < finished->io_amount; j++)
            {
                if (finished->io_done[j] == 0)
                {
                    finished->io_done[j] = 1;
                    break;
                }
            }

            io_progress[i] = 0;
            io_running[i] = queue_remove(io[i]);
        }
        else if (!io_running[i])
        {
            io_running[i] = queue_remove(io[i]);
        }

        if (io_running[i])
            io_progress[i]++;
    }

    fprintf(log_file, "Tick: %d\n", device_current_time);
    if (running)
        fprintf(log_file, "Running: %d\n", running->pid);
    else
        fprintf(log_file, "Running: none\n");
    fprintf(log_file, "High:\n");
    queue_print(log_file, high);
    fprintf(log_file, "Low:\n");
    queue_print(log_file, low);
    if (io_running[IO_DISK])
        fprintf(log_file, "Running disk: %d\n", io_running[IO_DISK]->pid);
    else
        fprintf(log_file, "Running disk: none\n");
    fprintf(log_file, "Disk:\n");
    queue_print(log_file, io[IO_DISK]);
    if (io_running[IO_TAPE])
        fprintf(log_file, "Running tape: %d\n", io_running[IO_TAPE]->pid);
    else
        fprintf(log_file, "Running tape: none\n");
    fprintf(log_file, "Tape:\n");
    queue_print(log_file, io[IO_TAPE]);
    if (io_running[IO_PRINTER])
        fprintf(log_file, "Running printer: %d\n", io_running[IO_PRINTER]->pid);
    else
        fprintf(log_file, "Running printer: none\n");
    fprintf(log_file, "Printer:\n");
    queue_print(log_file, io[IO_PRINTER]);
    fprintf(log_file, "---\n");

    if (running)
        running->progress++;
    device_current_time++;
}

int main(int argc, char **argv)
{
    srand((unsigned)time(NULL));

    generate_processes();
    high = queue_new();
    low = queue_new();
    for (int i = 0; i < IO_TYPES_AMOUNT; i++)
        io[i] = queue_new();

    fprintf(log_file, "\n--- Execução: ---\n");
    while (ended_processes != PROCESS_MAX_AMOUNT)
    {
        tick();
    }

    fprintf(log_file, "\n--- Vida dos processos: ---\n");
    for (int i = 0; i < PROCESS_MAX_AMOUNT; i++)
    {
        Process *p = &processes[i];
        fprintf(log_file, "PID: %d Start tick: %d End tick: %d Ticks taken: %d\n",
                p->pid, p->tick_start, p->tick_end, p->tick_end - p->tick_start);
    }

    queue_free(high);
    queue_free(low);
    for (int i = 0; i < IO_TYPES_AMOUNT; i++)
        queue_free(io[i]);
}
