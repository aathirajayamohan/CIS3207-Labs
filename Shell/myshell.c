#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "myshell.h"

#define MAX_INPUT_SIZE 40
#define MAX_PATH_SIZE 4

char error_message[30] = "An error has occured\n";
// max of 4 paths
char* paths[4];

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
            // for (int i = 0; i < numOfArgs; i++)
            // {
            //     printf("%s ", tokens[i]);
            // }
            // puts("");

            if (!builtInCommand(tokens, numOfArgs))
            {
                
                // fork the external command
                int pid = fork();
                if (pid < 0)
                {
                    puts("fork failed");
                    exit(1);
                }

                // child process
                if (pid == 0)
                {
                    int accessStatus = -1;
                    // check if process can run command from path
                    for (int i = 0; i < MAX_PATH_SIZE; i++)
                    {
                        if (paths[i] == NULL) break;

                        char* tempPath1 = strcat(paths[i], "/");
                        char* tempPath2 = strcat(tempPath1, tokens[0]);
                        printf("Checking path %s\n", tempPath2);

                        accessStatus = access(tempPath2, X_OK);
                        if (accessStatus == 0)
                        {
                            break;
                        }
                        else if (accessStatus < 0)
                        {
                            puts("check other path");
                        }
                    }

                    // checked all paths and still can't access command
                    // exit with an error
                    if (accessStatus < 0)
                    {
                        puts("can't access");
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        exit(1);
                    }

                    if (execvp(tokens[0], tokens) < 0)
                    {
                        perror("Could not execute command");
                        exit(1);
                    }
                }
                // parent process
                else
                {
                    waitpid(pid, NULL, 0);
                }
                
            }
            
            
        }
    }

    free(userInput);
    
    
    return 0;
}

int builtInCommand(char* tokens[], int numOfArgs)
{
    // quit the shell
    if (strcmp(tokens[0], "quit") == 0 || strcmp(tokens[0], "exit") == 0)
    {
        exit(0);
    }
    // clear the screen
    else if (strcmp(tokens[0], "clr") == 0)
    {
        printf("\e[1;1H\e[2J");
        return 1;
    }
    // add a path
    // fix later
    else if (strcmp(tokens[0], "path") == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            if (paths[i] == NULL)
            {
                paths[i] = tokens[1];
                break;
            }
        }
        return 1;
    }
    // change directories
    else if (strcmp(tokens[0], "cd") == 0)
    {
        // should only have one argument
        // 0 or 1 argument is an error
        if (numOfArgs == 2)
        {
            if (chdir(tokens[1]) != 0)
            {
                // failed to change directories
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
        }
        // no args or greater than 1 args passed to the function
        else
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
    
        return 1;
    }
    return 0;
}