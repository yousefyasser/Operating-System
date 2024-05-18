#include "shared.h"

void create_pcb(char *);
int loadProcessInstructionsToMemory(char *);

void create_process(char *priority, char *programName)
{
    create_pcb(priority);

    // leave 3 empty memory elements after pcb to store process variables
    firstFreeMemoryPos += 3;

    // read program text file line by line into memory locations for process instructions
    int processInstructionCount = loadProcessInstructionsToMemory(programName);
    int upperBoundPos = firstFreeMemoryPos - processInstructionCount - 3;

    // Adjust memory Upper Bound according to number of instructions
    char *memUpperBound = int_to_string(atoi(memory[upperBoundPos - 1].value) + 1 + processInstructionCount);
    memoryElement element = {"memUpperBound", memUpperBound};
    memory[upperBoundPos] = element;
}

void create_pcb(char *priority)
{
    // skip first 6 memory elements in the process (pcb) to get first variable position
    int lowerBound = firstFreeMemoryPos + 6;

    char *pcbElementsName[] = {"id", "state", "priority", "pc", "memLowerBound", "memUpperBound"};
    char *pcbElementsVal[] = {int_to_string(processesCount++), "READY", priority, int_to_string(lowerBound + 3), int_to_string(lowerBound), ""};

    for (int i = 0; i < sizeof(pcbElementsName) / sizeof(pcbElementsName[0]); i++)
    {
        memoryElement element = {pcbElementsName[i], deep_copy(pcbElementsVal[i])};
        memory[firstFreeMemoryPos++] = element;
    }
}

char *int_to_string(int num)
{
    char *str = malloc(sizeof(char) * 50);
    sprintf(str, "%d", num);
    return str;
}

char *deep_copy(char *str)
{
    char *copy = malloc(strlen(str) + 1);
    strcpy(copy, str);
    return copy;
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

        memoryElement element = {deep_copy(instructionName), deep_copy(line)};
        memory[firstFreeMemoryPos++] = element;
    }

    return instructionCount;
}

// get index in memory of memory element with name "memElemName" for process with process id "pid"
int get_index_in_memory(char *memElemName, char *pid)
{
    int inProcess = 0;
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        if (memory[i].name && strcmp(memory[i].name, "id") == 0 && strcmp(memory[i].value, pid) == 0)
        {
            inProcess = 1;
        }

        if (inProcess && memory[i].name && strcmp(memory[i].name, memElemName) == 0)
        {
            return i;
        }
    }

    return -1;
}

// fetch next instruction (using the process pc) for process with id: pid, increment its pc
char *get_next_instruction(char *pid)
{
    int idIndex = get_index_in_memory("id", pid);

    int oldPC = atoi(memory[idIndex + 3].value);
    memory[idIndex + 3].value = int_to_string(oldPC + 1);

    if (idIndex == -1)
        return "** process not found **";

    return memory[oldPC].value;
}