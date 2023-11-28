#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include "util.h"

int current_time;
int remaining_time;
int random_processes_amount;

Process processes[PROCESS_MAX_AMOUNT];
Process *in_execution_process;
Queue *high, *low;

Queue *io[IO_TYPES_AMOUNT];
Process *io_execution[IO_TYPES_AMOUNT];
int io_duration[IO_TYPES_AMOUNT] = {IO_DISK_DURATION, IO_TAPE_DURATION, IO_PRINTER_DURATION};
int io_progress[IO_TYPES_AMOUNT];

void initialize_process(Process *process)
{
	if (process->duration > 1)
	{
		process->io_amount = rand() % (IO_AMOUNT + 1);
		process->io_starts = malloc(process->io_amount * sizeof(int));
		process->io_types = malloc(process->io_amount * sizeof(int));
		process->io_status = malloc(process->io_amount * sizeof(int));

		for (int i = 0; i < process->io_amount; i++)
		{
			process->io_starts[i] = (rand() % process->duration) + 1;
			process->io_types[i] = rand() % IO_TYPES_AMOUNT;
			process->io_status[i] = IO_NOT_DONE;
		}
	}
	else
	{
		process->io_amount = 0;
	}
}

void create_random_processes()
{
	for (int i = 0; i < random_processes_amount; i++)
	{
		Process *process = &processes[i];
		process->pid = i;
		process->ppid = (i + 2 * (rand() % 100)) * 3;
		process->start = rand() % START_TIME_LIMIT;
		process->duration = (rand() % DURATION_LIMIT) + 1;
		process->progress = 0;
		process->status = READY;
		process->priority = HIGH_PRIORITY;

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

	printf("%d processos criados aleatoriamente com sucesso.\n\n", random_processes_amount);
}

void run_process()
{
	do
	{
		if (!is_empty_queue(high))
		{
			in_execution_process = remove_process_queue(high);
			printf("O processo (PID: %d) da fila de ALTA prioridade será executado.\n",
				   in_execution_process->pid);
		}
		else if (!is_empty_queue(low))
		{
			in_execution_process = remove_process_queue(low);
			printf("O processo (PID: %d) da fila de BAIXA prioridade será executado.\n",
				   in_execution_process->pid);
		}

		if (!in_execution_process)
		{
			break;
		}
		else if (!in_execution_process->progress)
		{
			in_execution_process->enqueued_time = current_time;
		}

		for (int i = 0; i < in_execution_process->io_amount; i++)
		{
			int start = in_execution_process->io_starts[i];
			int PROCESS_RUNNING_AND_IO_NOT_DONE =
				start == in_execution_process->progress && in_execution_process->io_status[i] == IO_NOT_DONE;

			if (PROCESS_RUNNING_AND_IO_NOT_DONE)
			{
				int type = in_execution_process->io_types[i];
				in_execution_process->io_status[i] = IO_IN_PROGRESS;
				add_process_queue(io[type], in_execution_process);
				in_execution_process = NULL;
				break;
			}
		}
	} while (!in_execution_process);
}

void kill_process()
{
	for (int i = 0; i < IO_TYPES_AMOUNT; i++)
	{
		if (io_progress[i] == io_duration[i])
		{
			Process *ended_process = io_execution[i];

			switch (i)
			{
			case IO_DISK:
				add_process_queue(low, ended_process);
				break;
			case IO_TAPE:
			case IO_PRINTER:
				add_process_queue(high, ended_process);
				break;
			}

			for (int j = 0; j < ended_process->io_amount; j++)
			{
				if (ended_process->io_status[j] == IO_IN_PROGRESS)
				{
					ended_process->io_status[j] = IO_DONE;
					break;
				}
			}

			io_progress[i] = 0;
			io_execution[i] = remove_process_queue(io[i]);
		}
		else if (!io_execution[i])
		{
			io_execution[i] = remove_process_queue(io[i]);
		}

		if (io_execution[i])
			io_progress[i]++;
	}
}

void scheduler()
{
	printf("Inserindo novos processos na fila...\n");
	for (int i = 0; i < random_processes_amount; i++)
	{
		Process *process = &processes[i];
		if (process->start == current_time)
		{
			printf("Processo (PID: %d) inserido na fila de ALTA prioridade.\n", process->pid);
			add_process_queue(high, process);
		}
	}
	printf("-------------------------------------\n");
	printf("Movendo processos interrompidos para a fila de baixa prioridade...\n");
	if (in_execution_process)
	{
		if (in_execution_process->progress == in_execution_process->duration)
		{
			remaining_time++;
			printf("Processo (PID: %d) finalizado.\n", in_execution_process->pid);
			in_execution_process->dequeued_time = current_time;
			in_execution_process = NULL;
		}
		else
		{
			printf("Processo (PID: %d) inserido na fila de BAIXA prioridade.\n", in_execution_process->pid);
			add_process_queue(low, in_execution_process);
		}
	}

	printf("=====================================\n");

	run_process();
	kill_process();

	printf("-------------------------------------\n");
	printf("(*) Tempo: %d\n", current_time);
	if (in_execution_process)
		highlighted_print(GREEN, "Processo em execução (PID: %d)\n\n", in_execution_process->pid);
	else
		printf("Nenhum processo em execução no momento.\n\n");
	highlighted_print(RED, "(->) Fila de ALTA prioridade:\n");
	print_queue(high);
	printf("\n");
	highlighted_print(YELLOW, "(->) Fila de BAIXA prioridade:\n");
	print_queue(low);
	printf("\n\n");
	if (io_execution[IO_DISK])
	{
		printf("Processo em I/O do Disco (PID: %d)\n", io_execution[IO_DISK]->pid);
		printf("Fila de I/O do Disco:\n");
		print_queue(io[IO_DISK]);
	}
	else
		printf("Nenhum processo está executando I/O do Disco no momento.\n");

	if (io_execution[IO_TAPE])
	{
		printf("Processo em I/O da Fita (PID: %d)\n", io_execution[IO_TAPE]->pid);
		printf("Fila de I/O da Fita:\n");
		print_queue(io[IO_TAPE]);
	}

	else
		printf("Nenhum processo está executando I/O da Fita no momento.\n");

	if (io_execution[IO_PRINTER])
	{
		printf("Processo em I/O da Impressora (PID: %d)\n", io_execution[IO_PRINTER]->pid);
		printf("Fila de I/O da Impressora:\n");
		print_queue(io[IO_PRINTER]);
	}
	else
		printf("Nenhum processo está executando I/O da Impressora no momento.\n");

	printf("\n");

	if (in_execution_process)
		in_execution_process->progress++;
	current_time++;
}

int main(int argc, char **argv)
{
	printf("=====================================\n");
	printf("Iniciando o algoritmo de escalonamento...\n");
	srand((unsigned)time(NULL));

	random_processes_amount = 1 + (rand() % PROCESS_MAX_AMOUNT);

	printf("Quantidade de processos selecionados aleatoriamente: %d\n", random_processes_amount);
	printf("=====================================\n");

	printf("Gerando processos aleatoriamente...\n");
	printf("----------------\n");
	create_random_processes();
	printf("=====================================\n");

	printf("Criando filas de prioridade...\n");
	high = new_queue();
	printf("Fila de prioridade ALTA criada com sucesso.\n");
	low = new_queue();
	printf("Fila de prioridade BAIXA criada com sucesso.\n");
	printf("=====================================\n");

	printf("Criando filas de I/O...\n");
	for (int i = 0; i < IO_TYPES_AMOUNT; i++)
	{
		io[i] = new_queue();
		printf("Fila de I/O para %s criada com sucesso.\n", get_enum_name(i));
	}
	printf("=====================================\n");

	printf("Executando o escalonador de processos...\n");
	while (remaining_time != random_processes_amount)
	{
		printf("=====================================\n");
		printf("Tempo restante (dispositivo): %d\n", random_processes_amount - remaining_time);
		scheduler();
		sleep(1);
	}
	printf("Escalonador de processos finalizado.\n");
	printf("=====================================\n");

	printf("\nResultado da execução:\n");

	int **processes_statistics = map_processes_to_statistics(random_processes_amount, processes);
	printf("----------------\n");
	show_processes_statistics(random_processes_amount, processes_statistics);

	free_queue(high);
	free_queue(low);
	for (int i = 0; i < IO_TYPES_AMOUNT; i++)
		free_queue(io[i]);

	free(processes_statistics);
}
