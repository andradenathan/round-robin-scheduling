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

Queue *queue_new(void);

void queue_free(Queue *q);

void queue_add(Queue *q, Process *p);

Process *queue_remove(Queue *q);

int queue_is_empty(Queue *q);

void queue_print(Queue *q);
