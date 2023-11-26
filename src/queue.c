#include <stdlib.h>
#include <stdio.h>

#include "queue.h"

Queue *new_queue(void)
{
	Queue *queue = malloc(sizeof(Queue));
	queue->head = NULL;
	return queue;
}

void free_queue(Queue *queue)
{
	if (queue->head)
	{
		Node *node = queue->head;
		while (node)
		{
			Node *old = node;
			node = node->next;
			free(old);
		}
	}
	free(queue);
}

void add_process_queue(Queue *queue, Process *process)
{
	Node *new = malloc(sizeof(Node));
	new->value = process;
	new->next = NULL;

	if (!queue->head)
	{
		queue->head = new;
	}
	else
	{
		Node *tail = queue->head;
		while (tail->next)
			tail = tail->next;

		// for (tail = q->head; tail->next; tail = tail->next)
		// 	;

		tail->next = new;
	}
}

Process *remove_process_queue(Queue *queue)
{
	if (!queue->head)
		return NULL;

	Node *old = queue->head;
	Process *process = old->value;
	queue->head = old->next;
	free(old);

	return process;
}

int is_empty_queue(Queue *queue)
{
	return queue->head == NULL;
}

void print_queue(Queue *queue)
{
	if (is_empty_queue(queue))
		printf("A fila está vazia no momento.\n");

	for (Node *node = queue->head; node; node = node->next)
	{
		printf("[pid: %d | progresso: %d / duração: %d]\n", node->value->pid, node->value->progress, node->value->duration);
		if (node->next != NULL)
			printf("-> ");
	}
}
