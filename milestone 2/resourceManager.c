#include <string.h>
#include <stdio.h>

#define MAX_VARIABLES_PER_PROGRAM 10

struct pcb
{
    int processId;
    char processState[10];
    int priority;
    int pc;
    int memoryBoundaries[2];
};

int tokenize(char *, char *[]);
char *get_program_variable(char *);
void set_program_variable(char *, char *);

char programVariables[MAX_VARIABLES_PER_PROGRAM][2][100];

// takes a txt file name and executes its content (program)
void run_program(const char *fileName)
{
    FILE *file = fopen(fileName, "r");
    if (file == NULL)
    {
        printf("Failed to open file %s\n", fileName);
        return;
    }

    char line[256];

    while (fgets(line, sizeof(line), file))
    {
        char *words[4];
        int num_tokens = tokenize(line, words);

        if (strcmp(words[0], "semWait") == 0)
        {
        }
        else if (strcmp(words[0], "semSignal") == 0)
        {
        }
        else if (strcmp(words[0], "assign") == 0)
        {
            // words[2] is either "input" that takes input from user and put it in variable in words[1]
            // or words[2] is a readfile instruction followed by words[3] (name of file) and put file content in variable in words[1]
            if (strcmp(words[2], "readFile"))
            {
            }
            else
            {
            }
        }
        else if (strcmp(words[0], "print") == 0)
        {
        }
        else if (strcmp(words[0], "printFromTo") == 0)
        {
        }
        else if (strcmp(words[0], "writeFile") == 0)
        {
        }
        else
        {
            printf("Unsupported Instruction: %s", words[0]);
        }
    }

    fclose(file);
}

int tokenize(char *line, char *tokens[])
{
    int num_tokens = 0;
    char *token = strtok(line, " ");
    while (token != NULL)
    {
        tokens[num_tokens] = token;
        num_tokens++;
        token = strtok(NULL, " ");
    }

    return num_tokens;
}

char *get_program_variable(char *variableName)
{
    for (int i = 0; i < MAX_VARIABLES_PER_PROGRAM; i++)
    {
        if (strcmp(programVariables[i][0], variableName) == 0)
        {
            return programVariables[i][1];
        }
    }

    return "variable not found";
}

void set_program_variable(char *variableName, char *variableValue)
{
    for (int i = 0; i < MAX_VARIABLES_PER_PROGRAM; i++)
    {
        if (strcmp(programVariables[i][0], variableName) == 0)
        {
            strcpy(programVariables[i][1], variableValue);
            return;
        }
        else if (strcmp(programVariables[i][0], "\0") == 0)
        {
            strcpy(programVariables[i][0], variableName);
            strcpy(programVariables[i][1], variableValue);
            return;
        }
    }

    printf("max variables per program reached");
}

int main()
{
    run_program("Program_3.txt");

    return 0;
}