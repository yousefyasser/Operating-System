#include "shared.h"

void print_memory();

int main()
{
    initialize_semaphores();

    // must be in ascending order by release time
    process p1 = {"1", "Program_1.txt", 0};
    process p2 = {"10", "Program_2.txt", 1};
    process p3 = {"20", "Program_3.txt", 6};
    process processes[] = {p1, p2, p3};
    char *chosenProcess[] = {"0", "0", "1", "0", "1", "0", "1"};

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
        char *chosenProcessID = chosenProcess[clockCycle];

        // run instruction at pc for the chosen process
        change_process_state(chosenProcessID, "RUNNING");
        char *instr = get_next_instruction(chosenProcessID);
        // printf("\n<> %s\n", instr);

        int blocked = 0;
        if (strcmp(instr, "** process not found **"))
            blocked = run_instruction(instr, chosenProcessID);

        change_process_state(chosenProcessID, blocked > 0 ? "BLOCKED" : "READY");

        if (blocked <= 0)
        {
            // increment pc for this process if not blocked
            int idIndex = get_index_in_memory("id", chosenProcessID);
            int oldPC = atoi(memory[idIndex + 3].value);
            memory[idIndex + 3].value = int_to_string(oldPC + 1);

            if (blocked < 0)
            {
                dequeue(resources[abs(blocked) - 1].blockedQueue);
            }
        }
        else
        {
            enqueue(resources[blocked - 1].blockedQueue, processes[atoi(chosenProcessID)]);
        }

        print_queue(resources[0].blockedQueue);
        printf("\n");
        print_queue(resources[1].blockedQueue);
        printf("\n");
        print_queue(resources[2].blockedQueue);
        printf("\n");

        clockCycle++;
        if (processesCount >= 3)
            break;
    }

    // printf("---------------------------------------------- Clock Cycle %d ----------------------------------------------\n", 7);
    // print_memory();

    destroy_semaphores();

    return 0;
}

void print_memory()
{
    int currProcess = 0;
    int lastNonNull = MEMORY_SIZE - 1;

    // Find the last non-null memory element
    while (lastNonNull >= 0 && memory[lastNonNull].name == NULL)
    {
        lastNonNull--;
    }

    for (int i = 0; i <= lastNonNull; i++)
    {
        if (memory[i].name && strcmp(memory[i].name, "id") == 0)
        {
            printf("Process %d:\n", currProcess++);
        }

        printf("%s: %s\n", memory[i].name, memory[i].value);

        if (i + 1 <= lastNonNull && memory[i + 1].name && strcmp(memory[i + 1].name, "id") == 0)
        {
            printf("\n-------------------------------------\n");
        }
    }
}