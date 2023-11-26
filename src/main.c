#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include "util.h"

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
		process->ppid = (i + 2 * (rand() % 100)) * 3;
		process->start = rand() % START_LIMIT;
		process->duration = (rand() % DURATION_LIMIT) + 1;
		process->progress = 0;

		initialize_process(process);

		printf("Processo: %d\n", i + 1);
		printf("PID %d\n", process->pid);
		printf("PPID %d\n", process->ppid);
		printf("Início %d\n", process->start);
		printf("Duração %d\n", process->duration);
		printf("Quantidade de IO %d\n", process->io_amount);
		for (int j = 0; j < process->io_amount; j++)
		{
			printf("Tipo: %s \t Inicio: %d\n", get_enum_name(process->io_types[j]), process->io_starts[j]);
		}
		printf("-------------\n");
		sleep(1);
	}

	printf("%d processos criados aleatoriamente com sucesso.\n\n", quantity);
}

void select_process_to_run()
{
	do
	{
		if (!queue_is_empty(high))
		{
			running = queue_remove(high);
			printf("O processo (PID: %d) da fila de ALTA prioridade será executado.\n", running->pid);
		}
		else if (!queue_is_empty(low))
		{
			running = queue_remove(low);
			printf("O processo (PID: %d) da fila de BAIXA prioridade será executado.\n", running->pid);
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

			if (start == running->progress && running->io_done[i] == IO_NOT_DONE)
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
	printf("Inserindo novos processos na fila...\n");
	for (int i = 0; i < PROCESS_AMOUNT; i++)
	{
		Process *process = &processes[i];
		if (process->start == current_time)
		{
			printf("Processo (PID: %d) inserido na fila de ALTA prioridade.\n", process->pid);
			queue_add(high, process);
		}
	}
	printf("-------------------------------------\n");
	printf("Movendo processos interrompidos para a fila de baixa prioridade...\n");
	if (running)
	{
		if (running->progress == running->duration)
		{
			remaining_time++;
			printf("Processo (PID: %d) finalizado.\n", running->pid);
			running->dequeued_time = current_time;
			running = NULL;
		}
		else
		{
			printf("Processo (PID: %d) inserido na fila de BAIXA prioridade.\n", running->pid);
			queue_add(low, running);
		}
	}

	printf("=====================================\n");
	// TODO: renomear os nomes das funções abaixo, não ficou claro ainda.
	select_process_to_run();
	select_io_to_processes();

	printf("-------------------------------------\n");
	printf("(*) Tempo do processo: %d\n", current_time);
	if (running)
		printf("Processo em execução (PID: %d)\n\n", running->pid);
	else
		printf("Nenhum processo em execução no momento.\n\n");
	printf("(-->) Fila de ALTA prioridade:\n");
	queue_print(high);
	printf("\n");
	printf("(-->) Fila de BAIXA prioridade:\n");
	queue_print(low);
	printf("\n\n");
	if (io_running[IO_DISK])
	{
		printf("Processo em I/O do Disco (PID: %d)\n", io_running[IO_DISK]->pid);
		printf("Fila de I/O do Disco:\n");
		queue_print(io[IO_DISK]);
	}
	else
		printf("Nenhum processo está executando I/O do Disco no momento.\n");

	if (io_running[IO_TAPE])
	{
		printf("Processo em I/O da Fita (PID: %d)\n", io_running[IO_TAPE]->pid);
		printf("Fila de I/O da Fita:\n");
		queue_print(io[IO_TAPE]);
	}

	else
		printf("Nenhum processo está executando I/O da Fita no momento.\n");

	if (io_running[IO_PRINTER])
	{
		printf("Processo em I/O da Impressora (PID: %d)\n", io_running[IO_PRINTER]->pid);
		printf("Fila de I/O da Impressora:\n");
		queue_print(io[IO_PRINTER]);
	}
	else
		printf("Nenhum processo está executando I/O da Fila no momento.\n");

	printf("\n");

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
	printf("Quantidade de processos selecionados aleatoriamente: %d\n", quantity);
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
		printf("=====================================\n");
		printf("Tempo restante (dispositivo): %d\n", remaining_time);
		calculate_scheduler();
	}
	printf("Escalonador de processos finalizado.\n");
	printf("=====================================\n");

	printf("\nEstatísticas:\n");

	int **processes_statistics = map_processes_to_statistics(quantity, processes);
	printf("----------------\n");
	show_processes_statistics(quantity, processes_statistics);

	queue_free(high);
	queue_free(low);
	for (int i = 0; i < IO_TYPES_AMOUNT; i++)
		queue_free(io[i]);

	free(processes_statistics);
}
