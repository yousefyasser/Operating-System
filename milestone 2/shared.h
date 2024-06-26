#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

#define MEMORY_SIZE 60
#define RESOURCES_SIZE 3

typedef struct
{
    char *program;
    int pid, releaseTime, quantum;
} process;

typedef struct
{
    char *name;
    process *data;
    int capacity, size, front, rear;
} queue;

typedef struct
{
    sem_t semaphore;
    queue *blockedQueue;
} resource;

typedef struct
{
    char *name;
    char *value;
} memoryElement;

extern memoryElement memory[];
extern int firstFreeMemoryPos;
extern int processesCount;
// semaphores for all available resources
extern resource resources[];

// function declarations (defined in ProcessCreation.c)
void create_process(char *programName);
char *int_to_string(int num);
char *deep_copy(char *str);
int get_index_in_memory(char *memElemName, char *pid);
char *get_next_instruction(char *pid);
void change_process_state(char *pid, char *newState);

// function declarations (defined in resourceManager.c)
int run_instruction(char *instruction, char *pid);
char *get_program_variable(char *pid, char *variableName);
void set_program_variable(char *pid, char *variableName, char *variableValue);
void initialize_semaphores();
void destroy_semaphores();

// function declarations (defined in priorityQueue.c)
queue *initialize_queue(char *name, int cap);
int is_empty(queue *q);
void enqueue(queue *q, process new_process);
process dequeue(queue *q);
void remove_process(queue *q, int pid);
process peek(queue *q);
void print_queue(queue *q);

#endif