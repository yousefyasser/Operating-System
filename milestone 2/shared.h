#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 60

typedef struct
{
    char *name;
    char *value;
} memoryElement;

extern memoryElement memory[];
extern int firstFreeMemoryPos;
extern int processesCount;

// function declarations (defined in ProcessCreation.c)
void create_process(char *priority, char *programName);
char *int_to_string(int num);
char *deep_copy(char *str);
int get_index_in_memory(char *memElemName, char *pid);
char *get_next_instruction(char *pid);

// function declarations (defined in resourceManager.c)
void run_instruction(char *instruction, char *pid);
char *get_program_variable(char *pid, char *variableName);
void set_program_variable(char *pid, char *variableName, char *variableValue);

#endif