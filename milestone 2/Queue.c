#include "shared.h"

queue *initialize_queue(char *name, int cap)
{
    queue *q = (queue *)malloc(sizeof(queue));
    q->name = name;
    q->capacity = cap;
    q->size = 0;
    q->front = 0;
    q->rear = cap - 1;
    q->data = (process *)malloc(q->capacity * sizeof(process));
    if (q->data == NULL)
    {
        printf("Memory allocation failed\n");
        exit(1);
    }
    return q;
}

int is_empty(queue *q)
{
    return (q->size == 0);
}

int is_full(queue *q)
{
    return (q->size == q->capacity);
}

void enqueue(queue *q, process new_process)
{
    // Check if the process is already in the queue
    for (int i = 0; i < q->size; i++)
    {
        if (q->data[(q->front + i) % q->capacity].pid == new_process.pid)
        {
            printf("FOUND\n");
            return;
        }
    }

    if (is_full(q))
    {
        printf("FULL\n");
        return;
    }

    q->rear = (q->rear + 1) % q->capacity;
    q->data[q->rear] = new_process;
    q->size = q->size + 1;
}

process dequeue(queue *q)
{
    if (is_empty(q))
    {
        printf("EMPTY\n");
        return (process){NULL, 0, 0};
    }

    process item = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size = q->size - 1;
    return item;
}

void remove_process(queue *q, int pid)
{
    if (is_empty(q))
    {
        printf("Queue is empty\n");
        return;
    }

    int originalSize = q->size;
    queue *tempQueue = initialize_queue("temp", q->capacity);
    for (int i = 0; i < originalSize; i++)
    {
        process item = dequeue(q);
        if (item.pid != pid)
        {
            enqueue(tempQueue, item);
        }
    }

    // Copy the temporary queue back to the original queue
    while (!is_empty(tempQueue))
    {
        enqueue(q, dequeue(tempQueue));
    }

    // Free the temporary queue
    free(tempQueue->data);
    free(tempQueue);
}

process peek(queue *q)
{
    return q->data[q->front];
}

void print_queue(queue *q)
{
    if (is_empty(q))
    {
        printf("Queue is empty\n");
        return;
    }

    printf("%s:\n", q->name);
    int count = q->size;
    int i = q->front;
    while (count > 0)
    {
        printf("Process: ID - %d, Program - %s, Release Time - %d\n", q->data[i].pid, q->data[i].program, q->data[i].releaseTime);
        i = (i + 1) % q->capacity;
        count--;
    }
    printf("\n\n");
}
