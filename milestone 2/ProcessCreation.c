#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 60

typedef struct
{
    char *name, *value;
} memoryElement;

memoryElement memory[MEMORY_SIZE];
int processesCount = 0;
int firstFreeMemoryPos = 0;

void create_process(char *, char *);
void create_pcb(char *);
int loadProcessInstructionsToMemory(char *);
void print_memory();

int main()
{
    create_process("1", "Program_1.txt");
    // print_memory();
    // printf("-------------------------------------\n");

    create_process("10", "Program_2.txt");
    // print_memory();
    // printf("-------------------------------------\n");

    create_process("20", "Program_3.txt");
    print_memory();

    return 0;
}

void create_process(char *priority, char *programName)
{
    create_pcb(priority);

    // leave 3 empty memory elements after pcb to store process variables
    firstFreeMemoryPos += 3;

    // read program text file line by line into memory locations for process instructions
    int processInstructionCount = loadProcessInstructionsToMemory(programName);
    int upperBoundPos = firstFreeMemoryPos - processInstructionCount - 3;

    // Adjust memory Upper Bound according to number of instructions
    char memUpperBound[50];
    sprintf(memUpperBound, "%d", atoi(memory[upperBoundPos - 1].value) + 3 + processInstructionCount - 1);

    char *valueCopy = malloc(strlen(memUpperBound) + 1);
    strcpy(valueCopy, memUpperBound);

    memoryElement element = {"memUpperBound", valueCopy};
    memory[upperBoundPos] = element;

    processesCount++;
}

void create_pcb(char *priority)
{
    // store first free memory position + 6 (for pcb) as a string in memLowerBound
    char memLowerBound[50];
    sprintf(memLowerBound, "%d", firstFreeMemoryPos + 6);

    char *pcbElementsName[] = {"id", "state", "priority", "pc", "memLowerBound", "memUpperBound"};
    char *pcbElementsVal[] = {"", "READY", priority, "0", memLowerBound, ""};

    for (int i = 0; i < sizeof(pcbElementsName) / sizeof(pcbElementsName[0]); i++)
    {
        char *valueCopy = malloc(strlen(pcbElementsVal[i]) + 1);
        strcpy(valueCopy, pcbElementsVal[i]);

        memoryElement element = {pcbElementsName[i], valueCopy};
        memory[firstFreeMemoryPos++] = element;
    }
}

int loadProcessInstructionsToMemory(char *programName)
{
    FILE *file = fopen(programName, "r");
    if (file == NULL)
    {
        printf("Failed to open file %s\n", programName);
        return -1;
    }

    char line[256];
    int instructionCount = 1;

    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n\r")] = 0; // remove newline and carriage return characters if they exist

        char instructionName[50];
        sprintf(instructionName, "Instruction %d", instructionCount++);

        char *nameCopy = malloc(strlen(instructionName) + 1);
        strcpy(nameCopy, instructionName);

        char *valueCopy = malloc(strlen(line) + 1);
        strcpy(valueCopy, line);

        memoryElement element = {nameCopy, valueCopy};
        memory[firstFreeMemoryPos++] = element;
    }

    return instructionCount;
}

void print_memory()
{
    int currProcess = 0;
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        if (i % 16 == 0)
        {
            printf("Process %d:\n", currProcess++);
        }

        printf("%s: %s\n", memory[i].name, memory[i].value);

        if (i % 16 == 15)
        {
            printf("\n");
        }
    }
}
