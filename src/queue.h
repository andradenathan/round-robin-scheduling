#include <stdlib.h>
#include <stdio.h>

enum
{
	IO_DISK = 0,
	IO_TAPE,
	IO_PRINTER,
};

typedef struct
{
	int pid;
	int ppid;
	int start;

	int enqueued_time;
	int dequeued_time;

	int duration;
	int progress;

	int io_amount;
	int *io_starts;
	int *io_types;
	int *io_done;
} Process;

typedef struct Node Node;

typedef struct Node
{
	Process *value;
	Node *next;
} Node;

typedef struct
{
	Node *head;
} Queue;

Queue *new_queue(void);

void free_queue(Queue *queue);

void add_process_queue(Queue *queue, Process *process);

Process *remove_process_queue(Queue *queue);

int is_empty_queue(Queue *queue);

void print_queue(Queue *queue);
