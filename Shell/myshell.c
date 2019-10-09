#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "myshell.h"

#define MAX_INPUT_SIZE 40

int main()
{
    char *userInput = malloc(MAX_INPUT_SIZE);
    char* tokens[MAX_INPUT_SIZE];

    
    while(1)
    {
        // greeting message
        printf("myshell> ");
        fgets(userInput, MAX_INPUT_SIZE, stdin);
        
        // remove the newline character for easier comparison later
        if ( (strlen(userInput) > 0) && (userInput[strlen(userInput)-1] == '\n'))
        {
            userInput[strlen(userInput)-1] = '\0';
        }

        // parses input into array
        int index = 0;
        int numOfArgs = 0;
        tokens[index] = strtok(userInput, " ");

        while (tokens[index] != NULL)
        {
            tokens[++index] = strtok(NULL, " ");
            numOfArgs++;
        }


        // check to see if command is a built in one
        if (numOfArgs > 0)
        {
            if (!builtInCommand(tokens))
            {
                puts("external command");
            }
            
            
        }
    }

    free(userInput);
    
    
    return 0;
}

int builtInCommand(char* tokens[])
{
    // quit the shell
    if (strcmp(tokens[0], "quit") == 0)
    {
        exit(0);
    }
    // clear the screen
    else if (strcmp(tokens[0], "clr") == 0)
    {
        printf("\e[1;1H\e[2J");
        return 1;
    }
    // change directories
    else if (strcmp(tokens[0], "cd") == 0)
    {
        // should only have one argument
        // 0 or 1 argument is an error
        
        return 1;
    }
    return 0;
}