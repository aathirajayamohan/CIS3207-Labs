#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "myshell.h"

#define MAX_INPUT_SIZE 40
#define MAX_PATH_SIZE 4

char error_message[30] = "An error has occured\n";
// max of 4 paths
char **paths;

int main()
{
    char *userInput = malloc(MAX_INPUT_SIZE);
    char* tokens[MAX_INPUT_SIZE];
    paths = (char**)malloc(MAX_PATH_SIZE * sizeof(char));

    paths[0] = "/bin";
    
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
            if (!builtInCommand(tokens, numOfArgs))
            {
                
                // fork the external command
                int pid = fork();
                if (pid < 0)
                {
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    exit(1);
                }

                // child process
                if (pid == 0)
                {
                    int accessStatus = -1;
                    char accessPath[40];

                    // check if process can run command from path
                    for (int i = 0; i < MAX_PATH_SIZE; i++)
                    {
                        if (paths[i] == NULL) break;

                        strcpy(accessPath, paths[i]);
                        strcat(accessPath, "/");
                        strcat(accessPath, tokens[0]);

                        accessStatus = access(accessPath, X_OK);
                        if (accessStatus == 0)
                        {
                            break;
                        }
                        // check other paths if they exits
                        else if (accessStatus < 0)
                        {
                            accessPath[0] = '\0';
                        }
                    }

                    // checked all paths and still can't access command
                    // exit with an error
                    if (accessStatus < 0)
                    {
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        exit(1);
                    }

                    if (execvp(tokens[0], tokens) < 0)
                    {
                        write(STDERR_FILENO, error_message, strlen(error_message));
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
    userInput = NULL;

    free(paths);
    paths = NULL;
    
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
    else if (strcmp(tokens[0], "path") == 0)
    {
        if (numOfArgs == 1)
        {
            // print the current paths
            for (int i = 0; i < MAX_PATH_SIZE; i++)
            {
                if (paths[i] == NULL) break;
                printf("%s ", paths[i]);
            }
            puts("");
        }
        else if (numOfArgs > 1)
        {
            // for (int i = 0; i < MAX_PATH_SIZE; i++)
            // {
            //     if (i > numOfArgs-1) break;
            //     //printf("%s\n", tokens[i]);
                
            //     paths[i] = tokens[1];
            // }
            
            for (int i = 1; i < numOfArgs; i++)
            {
                // printf("before: %s\n", paths[count]);
                paths[i-1] = tokens[i];
                // printf("after: %s\n", paths[count-1]);
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