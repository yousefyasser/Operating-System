#include "shared.h"

// Function to compare two processes based on priority (higher priority comes first)
int compare_processes(const void *a, const void *b)
{
    const process *processA = (const process *)a;
    const process *processB = (const process *)b;
    return strcmp(processB->priority, processA->priority); // Descending order
}

// Priority queue implementation using an array

queue *initialize_queue(int cap)
{
    queue *q = (queue *)malloc(sizeof(queue));
    q->capacity = cap;
    q->size = 0;
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
    return q->size == 0;
}

int is_full(queue *q)
{
    return q->size == q->capacity;
}

// Deep copy for process struct (excluding releaseTime - int)
process deep_copy_process(process p)
{
    process new_process;
    new_process.priority = strdup(p.priority);
    new_process.program = strdup(p.program);
    new_process.releaseTime = p.releaseTime; // No deep copy needed for primitive int
    return new_process;
}

void enqueue(queue *q, process new_process)
{
    // Check if the process is already in the queue
    for (int i = 0; i < q->size; i++)
    {
        if (compare_processes(&q->data[i], &new_process) == 0)
        {
            return;
        }
    }

    if (is_full(q))
    {
        return;
    }

    // Deep copy the process data before adding to the queue
    process to_enqueue = deep_copy_process(new_process);

    q->data[q->size] = to_enqueue;
    q->size++;

    // Heapify up for efficient priority order
    int i = q->size - 1;
    while (i > 0 && compare_processes(&q->data[(i - 1) / 2], &q->data[i]) < 0)
    {
        process temp = q->data[i];
        q->data[i] = q->data[(i - 1) / 2];
        q->data[(i - 1) / 2] = temp;
        i = (i - 1) / 2;
    }
}

process dequeue(queue *q)
{
    if (is_empty(q))
    {
        return (process){NULL, NULL, 0};
    }

    process top = q->data[0];

    // Move the last element to the root
    q->data[0] = q->data[q->size - 1];
    q->size--;

    // Heapify down to maintain heap property after removal
    int i = 0;
    while ((2 * i + 1) < q->size)
    {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int largest = i;

        if (left < q->size && compare_processes(&q->data[left], &q->data[largest]) > 0)
            largest = left;
        if (right < q->size && compare_processes(&q->data[right], &q->data[largest]) > 0)
            largest = right;

        if (largest != i)
        {
            process temp = q->data[i];
            q->data[i] = q->data[largest];
            q->data[largest] = temp;
            i = largest;
        }
        else
        {
            break;
        }
    }

    return top;
}

void print_queue(queue *q)
{
    if (q->size == 0)
    {
        printf("Queue is empty\n");
        return;
    }

    // Create a copy of the queue
    queue sortedQueue;
    memcpy(&sortedQueue, q, sizeof(queue));

    // Sort the queue based on priority
    for (int i = 0; i < sortedQueue.size - 1; i++)
    {
        for (int j = 0; j < sortedQueue.size - i - 1; j++)
        {
            if (strcmp(sortedQueue.data[j].priority, sortedQueue.data[j + 1].priority) > 0)
            {
                // Swap
                process temp = sortedQueue.data[j];
                sortedQueue.data[j] = sortedQueue.data[j + 1];
                sortedQueue.data[j + 1] = temp;
            }
        }
    }

    // Print the sorted queue
    printf("Queue: ");
    for (int i = 0; i < sortedQueue.size; i++)
    {
        printf("Process: Priority - %s, Program - %s, Release Time - %d\n", sortedQueue.data[i].priority, sortedQueue.data[i].program, sortedQueue.data[i].releaseTime);
    }
    printf("\n");
}
