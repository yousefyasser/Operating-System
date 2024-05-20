#include "shared.h"
#define PROCESSES_COUNT 3

void print_all_queues(queue *, queue *);
void print_memory();

int main()
{
    initialize_semaphores();

    queue *generalBlockedQueue = initialize_queue("General Blocked Queue", 10);
    queue *readyQueue = initialize_queue("Ready Queue", 10);

    // must be in ascending order by release time
    process p1 = {"Program_1.txt", 0, 0, 1};
    process p2 = {"Program_2.txt", 1, 1, 2};
    process p3 = {"Program_3.txt", 2, 6, 3};

    // take user input for release times
    printf("Enter release times for all 3 processes (in order):\n");
    scanf("%d", &p1.releaseTime);
    scanf("%d", &p2.releaseTime);
    scanf("%d", &p3.releaseTime);

    process processes[] = {p1, p2, p3};

    int clockCycle = 0;
    int quantumRem = 0;
    int processFinished = 0;

    while (1)
    {
        printf("---------------------------------------------- Clock Cycle %d ----------------------------------------------\n", clockCycle);

        // Check if a new process arrived
        if (processes[processesCount].releaseTime == clockCycle)
        {
            enqueue(readyQueue, processes[processesCount]);
            create_process(processes[processesCount].program);
        }

        if (is_empty(readyQueue) && processesCount < PROCESSES_COUNT)
        {
            printf("No Processes to execute\n");
            clockCycle++;
            continue;
        }

        if (quantumRem == 0)
        {
            if (!processFinished)
            {
                process toTheBack = dequeue(readyQueue);
                enqueue(readyQueue, toTheBack);
            }

            processFinished = 0;
            process frontOfQueue = peek(readyQueue);
            quantumRem = frontOfQueue.quantum;
        }

        // Scheduler chooses a process to execute
        process frontOfQueue = peek(readyQueue);
        char *chosenProcessID = int_to_string(frontOfQueue.pid);

        printf("Process currently running: %s\n\n", chosenProcessID);

        // run instruction at pc for the chosen process
        change_process_state(chosenProcessID, "RUNNING");
        char *instr = get_next_instruction(chosenProcessID);
        printf("<> Instruction currently executing: %s\n", instr);

        printf("\n**************************************************************\n");
        printf("Queues after process scheduling event:\n");
        print_all_queues(generalBlockedQueue, readyQueue);
        printf("\n**************************************************************\n");

        int blocked = 0;
        if (strcmp(instr, "** process not found **"))
            blocked = run_instruction(instr, chosenProcessID);

        change_process_state(chosenProcessID, blocked > 0 ? "BLOCKED" : "READY");

        if (blocked <= 0)
        {
            // increment pc for this process if not blocked
            int idIndex = get_index_in_memory("id", chosenProcessID);
            int oldPC = atoi(memory[idIndex + 2].value);
            memory[idIndex + 2].value = int_to_string(oldPC + 1);

            // sem signal release blocked process from resource blocked queue and general blocked queue and put it in ready queue
            if (blocked < 0 && !is_empty(resources[abs(blocked) - 1].blockedQueue))
            {
                process p = dequeue(resources[abs(blocked) - 1].blockedQueue);
                remove_process(generalBlockedQueue, p.pid);
                enqueue(readyQueue, p);
                change_process_state(int_to_string(p.pid), "READY");
            }
        }
        else // sem wait put process in blocked queue (general and resource) and remove from ready queue
        {
            enqueue(resources[blocked - 1].blockedQueue, processes[atoi(chosenProcessID)]);
            enqueue(generalBlockedQueue, processes[atoi(chosenProcessID)]);
            remove_process(readyQueue, atoi(chosenProcessID));

            printf("\n**************************************************************\n");
            printf("Queues after process blocking event:\n");
            print_all_queues(generalBlockedQueue, readyQueue);
            printf("\n**************************************************************\n");
        }

        int pcIndex = get_index_in_memory("pc", chosenProcessID);
        int memUpperBound = get_index_in_memory("memUpperBound", chosenProcessID);

        if (atoi(memory[pcIndex].value) > atoi(memory[memUpperBound].value))
        {
            change_process_state(chosenProcessID, "FINISHED");
            dequeue(readyQueue);
            processFinished = 1;

            printf("\n**************************************************************\n");
            printf("Queues after process finished:\n");
            print_all_queues(generalBlockedQueue, readyQueue);
            printf("\n**************************************************************\n");
        }

        quantumRem--;
        clockCycle++;

        if (is_empty(readyQueue) && processesCount >= PROCESSES_COUNT)
        {
            break;
        }

        print_memory();
        printf("\n");
    }

    destroy_semaphores();

    return 0;
}

void print_all_queues(queue *generalBlockedQueue, queue *readyQueue)
{
    print_queue(resources[0].blockedQueue);
    print_queue(resources[1].blockedQueue);
    print_queue(resources[2].blockedQueue);
    print_queue(generalBlockedQueue);
    print_queue(readyQueue);
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