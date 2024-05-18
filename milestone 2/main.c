#include "shared.h"

void print_memory();

int main()
{
    typedef struct
    {
        char *priority;
        char *program;
        int releaseTime;
    } process;

    // must be in ascending order by release time
    process p1 = {"1", "Program_1.txt", 0};
    process p2 = {"10", "Program_2.txt", 3};
    process p3 = {"20", "Program_3.txt", 5};
    process processes[] = {p1, p2, p3};

    int clockCycle = 0;
    while (1)
    {
        // Check if a new process arrived
        if (processes[processesCount].releaseTime == clockCycle)
        {
            create_process(processes[processesCount].priority, processes[processesCount].program);
            // printf("---------------------------------------------- Clock Cycle %d ----------------------------------------------\n", clockCycle);
            // print_memory();
        }

        // Scheduler chooses a process to execute
        char *chosenProcessID = "0";

        // run instruction at pc for the chosen process
        char *instr = get_next_instruction(chosenProcessID);
        // printf("\n<> %s\n", instr);

        if (strcmp(instr, "** process not found **"))
            run_instruction(instr, chosenProcessID);

        clockCycle++;
        if (processesCount >= 3)
            break;
    }

    print_memory();

    return 0;
}

void print_memory()
{
    int currProcess = 0;
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        if (memory[i].name && strcmp(memory[i].name, "id") == 0)
        {
            printf("Process %d:\n", currProcess++);
        }

        printf("%s: %s\n", memory[i].name, memory[i].value);

        if (i + 1 < MEMORY_SIZE && memory[i + 1].name && strcmp(memory[i + 1].name, "id") == 0)
        {
            printf("\n-------------------------------------\n");
        }
    }
}