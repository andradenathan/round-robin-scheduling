#include <stdio.h>
#include <stdlib.h>

typedef struct Process
{
    int pid;
    int ppid;
    int priority;
    int status;

    int service_time;
    IO *io;
} Process;

typedef struct IO
{
    Queue *device_queue;
    int priority;
    int time;
} IO;

typedef struct Device
{
    char *name;
    int duration;
    Process *process;

} Device;

typedef struct ProcessQueue
{
    Process *process;
    struct ProcessQueue *next;
} ProcessQueue;

typedef struct Queue
{
    ProcessQueue *start;
    ProcessQueue *end;
    int size;
} Queue;

Process *new_process(
    int pid,
    int ppid,
    int priority,
    int status,
    int service_time,
    IO *io)
{
    printf("Criando novo processo %d...\n", pid);
    Process *process = (Process *)malloc(sizeof(Process));
    process->pid = pid;
    process->ppid = ppid;
    process->priority = priority;
    process->status = status;
    process->service_time = service_time;
    process->io = io;
    return process;
}

Device *new_device(int time, char *name)
{
    Device *device = (Device *)malloc(sizeof(Device));
    device->name = name;
    device->duration = time;
    return device;
}

Queue *new_queue()
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->start = NULL;
    return queue;
}

void enqueue(Queue *queue, Process *process)
{
    ProcessQueue *process_queue = (ProcessQueue *)malloc(sizeof(ProcessQueue));
    process_queue->process = process;

    if (queue->end == NULL)
        queue->end = process_queue;

    else
    {
        queue->end->next = process_queue;
        queue->end = process_queue;
    }

    if (queue->start == NULL)
        queue->start = process_queue;

    queue->size++;
}

Process *dequeue(Queue *queue)
{
    if (queue->start == NULL)
        return NULL;

    Process *process = queue->start->process;
    queue->start = queue->start->next;
    queue->size--;
    free(queue->start);

    return process;
}

int main(int argc, char *argv[])
{
    return 0;
}