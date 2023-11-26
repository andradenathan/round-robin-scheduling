#include <stdlib.h>
#include <stdio.h>

#include "queue.h"

Queue *queue_new(void)
{
	Queue *new = malloc(sizeof(Queue));
	new->head = NULL;
	return new;
}

void queue_free(Queue *q)
{
	if (q->head)
	{
		Node *node = q->head;
		while (node)
		{
			Node *old = node;
			node = node->next;
			free(old);
		}
	}
	free(q);
}

void queue_add(Queue *q, Process *p)
{
	Node *new = malloc(sizeof(Node));
	new->value = p;
	new->next = NULL;

	if (!q->head)
	{
		q->head = new;
	}
	else
	{
		Node *tail;
		for (tail = q->head; tail->next; tail = tail->next)
			;

		tail->next = new;
	}
}

Process *queue_remove(Queue *q)
{
	if (!q->head)
		return NULL;

	Node *old = q->head;
	Process *ret = old->value;
	q->head = old->next;
	free(old);

	return ret;
}

int queue_is_empty(Queue *q)
{
	return q->head == NULL;
}

void queue_print(Queue *q)
{
	if (queue_is_empty(q))
		printf("A fila está vazia no momento.\n");

	for (Node *node = q->head; node; node = node->next)
	{
		printf("%d (%d/%d) ", node->value->pid, node->value->progress, node->value->duration);
		if (node->next != NULL)
			printf("-> ");
	}
}
