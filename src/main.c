#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include "util.h"

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

int current_time;
int remaining_time;
int quantity;

Process processes[PROCESS_AMOUNT];
Process *running;
Queue *high, *low;

Queue *io[IO_TYPES_AMOUNT];
Process *io_running[IO_TYPES_AMOUNT];
int io_duration[IO_TYPES_AMOUNT] = {IO_DISK_DURATION, IO_TAPE_DURATION, IO_PRINTER_DURATION};
int io_progress[IO_TYPES_AMOUNT];

void initialize_process(Process *process)
{
	if (process->duration > 1)
	{
		process->io_amount = rand() % (IO_LIMIT + 1);
		process->io_starts = malloc(process->io_amount * sizeof(int));
		process->io_types = malloc(process->io_amount * sizeof(int));
		process->io_done = malloc(process->io_amount * sizeof(int));

		for (int i = 0; i < process->io_amount; i++)
		{
			process->io_starts[i] = (rand() % process->duration) + 1;
			process->io_types[i] = rand() % IO_TYPES_AMOUNT;
			process->io_done[i] = IO_NOT_DONE;
		}
	}
	else
	{
		process->io_amount = 0;
	}
}

void create_random_processes()
{
	for (int i = 0; i < quantity; i++)
	{
		Process *process = &processes[i];
		process->pid = i;
		process->start = rand() % START_LIMIT;
		process->duration = (rand() % DURATION_LIMIT) + 1;
		process->progress = 0;

		initialize_process(process);

		printf("PID %d\n", process->pid);
		printf("Início %d\n", process->start);
		printf("Duração %d\n", process->duration);
		printf("Quantidade IO %d\n", process->io_amount);
		for (int j = 0; j < process->io_amount; j++)
		{
			printf("Tipo: %d Inicio: %d\n", process->io_types[j], process->io_starts[j]);
		}
		printf("-------------\n");
		sleep(1);
	}

	printf("%d processos gerados aleatoriamente com sucesso.\n\n", quantity);
}

void select_process_to_run()
{
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
			running->enqueued_time = current_time;
		}

		for (int i = 0; i < running->io_amount; i++)
		{
			int start = running->io_starts[i];

			if (start == running->progress && running->io_done[i] == -1)
			{
				int type = running->io_types[i];
				running->io_done[i] = IO_IN_PROGRESS;
				queue_add(io[type], running);
				running = NULL;
				break;
			}
		}
	} while (!running);
}

void select_io_to_processes()
{
	for (int i = 0; i < IO_TYPES_AMOUNT; i++)
	{
		if (io_progress[i] == io_duration[i])
		{
			Process *ended_process = io_running[i];

			switch (i)
			{
			case IO_DISK:
				queue_add(low, ended_process);
				break;
			case IO_TAPE:
			case IO_PRINTER:
				queue_add(high, ended_process);
				break;
			}

			for (int j = 0; j < ended_process->io_amount; j++)
			{
				if (ended_process->io_done[j] == IO_IN_PROGRESS)
				{
					ended_process->io_done[j] = IO_DONE;
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
}

void calculate_scheduler()
{
	printf("Rodando o escalonador de processos...\n");
	/* Coloca processos novos na fila */
	for (int i = 0; i < PROCESS_AMOUNT; i++)
	{
		Process *process = &processes[i];
		if (process->start == current_time)
			queue_add(high, process);
	}

	/* Move processos interrompidos para a fila de baixa prioridade */
	if (running)
	{
		if (running->progress == running->duration)
		{
			remaining_time++;
			running->dequeued_time = current_time;
			running = NULL;
		}
		else
		{
			queue_add(low, running);
		}
	}

	select_process_to_run();
	select_io_to_processes();

	printf("Tick: %d\n", current_time);
	if (running)
		printf("Running: %d\n", running->pid);
	else
		printf("Running: none\n");
	printf("High:\n");
	queue_print(high);
	printf("Low:\n");
	queue_print(low);
	if (io_running[IO_DISK])
		printf("Running disk: %d\n", io_running[IO_DISK]->pid);
	else
		printf("Running disk: none\n");
	printf("Disk:\n");
	queue_print(io[IO_DISK]);
	if (io_running[IO_TAPE])
		printf("Running tape: %d\n", io_running[IO_TAPE]->pid);
	else
		printf("Running tape: none\n");
	printf("Tape:\n");
	queue_print(io[IO_TAPE]);
	if (io_running[IO_PRINTER])
		printf("Running printer: %d\n", io_running[IO_PRINTER]->pid);
	else
		printf("Running printer: none\n");
	printf("Printer:\n");
	queue_print(io[IO_PRINTER]);
	printf("---\n");

	if (running)
		running->progress++;
	current_time++;
}

int main(int argc, char **argv)
{
	printf("=====================================\n");
	printf("Iniciando o algoritmo de escalonamento...\n");
	srand((unsigned)time(NULL));

	quantity = 1 + (rand() % PROCESS_AMOUNT);
	printf("Quantidade de processos selecionada aleatoriamente: %d\n", quantity);
	printf("=====================================\n");

	printf("Gerando processos aleatoriamente...\n");
	printf("----------------\n");
	create_random_processes();
	printf("=====================================\n");

	printf("Criando filas de prioridade...\n");
	high = queue_new();
	printf("Fila de prioridade ALTA criada com sucesso.\n");
	low = queue_new();
	printf("Fila de prioridade BAIXA criada com sucesso.\n");
	printf("=====================================\n");

	printf("Criando filas de I/O...\n");
	for (int i = 0; i < IO_TYPES_AMOUNT; i++)
	{
		io[i] = queue_new();
		printf("Fila de I/O para %s criada com sucesso.\n", get_enum_name(i));
	}
	printf("=====================================\n");

	printf("Executando o escalonador de processos...\n");
	while (remaining_time != quantity)
	{
		printf("Tempo restante: %d\n", quantity - remaining_time);
		calculate_scheduler();
		// sleep(1);
	}

	// printf("\n--- Vida dos processos: ---\n");
	// for (int i = 0; i < quantity; i++)
	// {
	// 	Process *p = &processes[i];
	// 	printf("PID: %d Start tick: %d End tick: %d Ticks taken: %d\n",
	// 		   p->pid, p->enqueued_time, p->dequeued_time, p->dequeued_time - p->enqueued_time);
	// }

	// queue_free(high);
	// queue_free(low);
	// for (int i = 0; i < IO_TYPES_AMOUNT; i++)
	// queue_free(io[i]);
}
