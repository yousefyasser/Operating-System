#include <semaphore.h>
#include "shared.h"

// functions declaration
int tokenize(char *, char *[]);
char *readfileContent(char *);
void writeToNewFile(char *, char *);
void wait_or_signal_semaphore(char *, int);

// semaphores for all available resources
sem_t userInputSem, userOutputSem, fileSem;

// takes an instruction (from program running in process with id pid) as a string and executes it
void run_instruction(char *instruction, char *pid)
{
    char *words[4];
    int num_tokens = tokenize(deep_copy(instruction), words);

    if (strcmp(words[0], "semWait") == 0)
    {
        // wait_or_signal_semaphore(words[1], 1);
    }
    else if (strcmp(words[0], "semSignal") == 0)
    {
        // wait_or_signal_semaphore(words[1], 0);
    }
    else if (strcmp(words[0], "assign") == 0)
    {
        // words[2] is either "input" that takes input from user and put it in variable in words[1]
        // or words[2] is a readfile instruction followed by words[3] (name of file) and put file content in variable in words[1]
        if (strcmp(words[2], "readFile") == 0)
        {
            char *subfileName = get_program_variable(pid, words[3]);
            char *fileContent = readfileContent(subfileName);
            set_program_variable(pid, words[1], fileContent == NULL ? "" : fileContent);
        }
        else
        {
            char input[100];
            printf("Please enter a value: ");
            scanf("%s", input);
            set_program_variable(pid, words[1], input);
        }
    }
    else if (strcmp(words[0], "print") == 0)
    {
        char *val = get_program_variable(pid, words[1]);
        printf("%s", val);
    }
    else if (strcmp(words[0], "printFromTo") == 0)
    {
        int from = atoi(get_program_variable(pid, words[1]));
        int to = atoi(get_program_variable(pid, words[2]));

        for (int i = from; i <= to; i++)
        {
            printf("%i ", i);
        }
    }
    else if (strcmp(words[0], "writeFile") == 0)
    {
        char *subfileName = get_program_variable(pid, words[1]);
        char *dataToWrite = get_program_variable(pid, words[2]);

        writeToNewFile(subfileName, dataToWrite);
    }
    else
    {
        printf("Unsupported Instruction: %s\n", words[0]);
    }
}

int tokenize(char *line, char *tokens[])
{
    int num_tokens = 0;
    char *token = strtok(line, " ");
    while (token != NULL)
    {
        token[strcspn(token, "\n\r")] = 0; // remove newline and carriage return characters if they exist
        tokens[num_tokens] = token;
        num_tokens++;
        token = strtok(NULL, " ");
    }

    return num_tokens;
}

// returns all lines of the file in a single string
char *readfileContent(char *subfileName)
{
    FILE *subfile = fopen(subfileName, "r");
    if (subfile == NULL)
    {
        printf("Failed to open file %s\n", subfileName);
        return NULL;
    }

    char *fileContent = NULL;
    char subfileLine[256];
    size_t len = 0;

    while (fgets(subfileLine, sizeof(subfileLine), subfile))
    {
        size_t line_len = strlen(subfileLine);
        fileContent = realloc(fileContent, len + line_len + 1); // allocate enough memory for the current fileContent and the new line
        strcpy(fileContent + len, subfileLine);                 // copy the new line to the end of the current fileContent
        len += line_len;                                        // update the length of the fileContent
    }

    return fileContent;
}

// writes dataToWrite string into a newly created file with name subfileName
void writeToNewFile(char *subfileName, char *dataToWrite)
{
    FILE *file = fopen(subfileName, "w");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return;
    }
    fprintf(file, "%s", dataToWrite);
    fclose(file);
}

char *get_program_variable(char *pid, char *variableName)
{
    // first variable position for process pid is stored in its memory lower bound in its pcb
    int lowerBoundIndex = get_index_in_memory("memLowerBound", pid);
    int start = atoi(memory[lowerBoundIndex].value);

    for (int i = start; i < start + 3; i++)
    {
        if (memory[i].name && strcmp(memory[i].name, variableName) == 0)
        {
            return memory[i].value;
        }
    }

    return "variable not found";
}

void set_program_variable(char *pid, char *variableName, char *variableValue)
{
    // first variable position for process pid is stored in its memory lower bound in its pcb
    int lowerBoundIndex = get_index_in_memory("memLowerBound", pid);
    int start = atoi(memory[lowerBoundIndex].value);

    int firstEmptyPos = -1;
    for (int i = start; i < start + 3; i++)
    {
        if (memory[i].name != NULL)
        {
            if (strcmp(memory[i].name, variableName) == 0)
            {
                strcpy(memory[i].value, variableValue);
                return;
            }
        }
        else if (firstEmptyPos == -1)
        {
            firstEmptyPos = i;
        }
    }

    if (firstEmptyPos == -1)
    {
        printf("\n!!! NO SPACE FOR NEW VARIABLES FOR PROCESS %s !!!\n", pid);
        return;
    }

    memoryElement elem = {deep_copy(variableName), deep_copy(variableValue)};
    memory[firstEmptyPos] = elem;
}

//      SEMAPHORE FUNCTIONS

void initialize_semaphores()
{
    sem_init(&userInputSem, 1, 1);
    sem_init(&userOutputSem, 1, 1);
    sem_init(&fileSem, 1, 1);
}

// wait = 0 means sem_signal the semaphore with semName else sem_wait
void wait_or_signal_semaphore(char *semName, int wait)
{
    if (strcmp(semName, "userInput"))
    {
        wait ? sem_wait(&userInputSem) : sem_post(&userInputSem);
    }
    else if (strcmp(semName, "userOutput"))
    {
        wait ? sem_wait(&userOutputSem) : sem_post(&userOutputSem);
    }
    else if (strcmp(semName, "file"))
    {
        wait ? sem_wait(&fileSem) : sem_post(&fileSem);
    }
    else
    {
        printf("Incorrect resource name");
    }
}

void destroy_semaphores()
{
    sem_destroy(&userInputSem);
    sem_destroy(&userOutputSem);
    sem_destroy(&fileSem);
}