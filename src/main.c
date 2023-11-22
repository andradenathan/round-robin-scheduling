#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_IO 3
#define MAX_PROCESS 6
#define READY 1
#define LOW_PRIORITY 1
#define HIGH_PRIORITY 2

typedef struct Process Process;
typedef struct Device Device;
typedef struct Queue Queue;
typedef struct ProcessQueue ProcessQueue;
typedef struct IO IO;

struct Process
{
    int pid;
    int ppid;
    int priority;
    int status;

    int enqueued_time;
    int service_time;
    IO *io;
};

struct Device
{
    char *name;
    int duration;
    Process *process;
};

struct IO
{
    Queue *device_queue;
    int priority;
    int time;
};

struct ProcessQueue
{
    Process *process;
    struct ProcessQueue *next;
};

struct Queue
{
    ProcessQueue *start;
    ProcessQueue *end;
    int size;
};

Process *new_process(
    int pid,
    int ppid,
    int service_time,
    IO *io)
{
    printf("Criando novo processo (PID: %d)...\n", pid);
    Process *process = (Process *)malloc(sizeof(Process));
    process->pid = pid;
    process->ppid = ppid;
    process->priority = HIGH_PRIORITY;
    process->status = READY;
    process->service_time = service_time;
    process->io = io;
    return process;
}

Process *new_random_process(Queue *queue)
{
    printf("Gerando processo aleatório...\n");

    int pid,
        ppid,
        service_time,
        enqueued_time,
        io_num;

    srand((unsigned)time(NULL));
    int quantity = 1 + (rand() % MAX_PROCESS);
    int current_index = 0;
    Process *processes = (Process *)malloc(sizeof(Process) * MAX_PROCESS);

    for (int index = 0; index < quantity; index++)
    {
        pid = index + 1;
        ppid = 2 + (index * 2);
        service_time = 1 + (rand() % 10);
        io_num = (rand() % MAX_IO) % service_time;

        if (service_time < MAX_IO + 1)
        {
            io_num = 0;
        }
        else
        {
        }

        Process *process = new_process(pid, ppid, service_time, NULL);
        processes[current_index] = *process;
        current_index++;
    }
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