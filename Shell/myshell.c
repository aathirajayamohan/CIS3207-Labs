#include "myshell.h"

enum shell_mode mode;

char error_message[30] = "An error has occured\n";
char *paths[MAX_PATH_SIZE];
char* pipeCommand;
char* backgroundCommand;

// flags
int foundInputRedirect;
int foundExecInBackground;
int isPaused;

int main(int argc, char **argv)
{
    char *userInput = malloc(MAX_INPUT_SIZE);
    
    
    FILE *fp = NULL;
    pipeCommand = NULL;
    backgroundCommand = NULL;
    
    isPaused = 0;
    
    // the default path
    paths[0] = "/bin";
    
    // check to see if a file was pass to the shell
    // if one was then set the bash mode accordingly
    if (argc > 1)
        mode = BATCH;
    else
        mode = NORMAL;
    
    
    
    while(1)
    {
        if (mode == NORMAL)
        {
            // greeting message
            if (!isPaused)
                greeting();
            fgets(userInput, MAX_INPUT_SIZE, stdin);
            run(userInput);
        }
        else
        {
            // batch mode
            fp = fopen(argv[1], "r");
            if (fp == NULL)
            {
                // failed to open file
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(1);
            }
            
            while (fgets(userInput, MAX_INPUT_SIZE, (FILE*)fp))
            {
                run(userInput);
            }
            // finished reading the file
            // exit the shell
            fclose(fp);
            break;
        }
        
    }
    
    free(userInput);
    userInput = NULL;
    
    return 0;
}

void greeting()
{
    char* workingPath = NULL;
    char buffer[PATH_MAX + 1];

    workingPath = getcwd(buffer, PATH_MAX + 1);
    if (workingPath == NULL)
    {
        printf("myshell> ");
    } 
    else
    {
        printf("myshell> ");
        //printf("myshell %s > ", workingPath);
    }
    
}

void run(char *userInput)
{
    char* tokens[MAX_INPUT_SIZE];
    
    // remove the newline character for easier comparison later
    if ( (strlen(userInput) > 0) && (userInput[strlen(userInput)-1] == '\n'))
    {
        userInput[strlen(userInput)-1] = '\0';
    }
    
    // locate any | or &
    pipeCommand = strchr(userInput, '|');
    backgroundCommand = strchr(userInput, '&');
    
    foundInputRedirect = 0;
    foundExecInBackground = 0;
    
    // parses input into array
    int index = 0;
    int numOfArgs = 0;
    tokens[index] = strtok(userInput, " ");
    
    while (tokens[index] != NULL)
    {
        if (strcmp(tokens[index], ">") == 0 || strcmp(tokens[index], "<") == 0)
            foundInputRedirect = 1;
        else if (strcmp(tokens[index], "&") == 0)
            foundExecInBackground = 1;
        
        tokens[++index] = strtok(NULL, " ");
        numOfArgs++;
    }
    
    // check to see if command is a built in one
    if (numOfArgs > 0 && !isPaused)
    {
        if (pipeCommand != NULL)
        {
            pipeExec(tokens, numOfArgs);
        }
        else if (foundInputRedirect)
        {
            inputRedirect(tokens, numOfArgs);
        }
        else if (foundExecInBackground)
        {
            backgroundExec(tokens, numOfArgs);
        }
        else if (!builtInCommand(tokens, numOfArgs))
        {
            execCommand(tokens, numOfArgs, 0);
        }
    }
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
    else if (strcmp(tokens[0], "help") == 0)
    {
        char* helpFile = malloc(1510 * sizeof(char*));
        FILE* fp = NULL;
        
        fp = fopen("readme", "r");

        if (fp == NULL)
        {
            // failed to open readme
            write(STDERR_FILENO, error_message, strlen(error_message));
            //exit(1);
        }
        
        while (fgets(helpFile, MAX_INPUT_SIZE, (FILE*)fp))
        {
            printf("%s", helpFile);
        }
        puts("");

        return 1;
    }
    // prints all of the environment variables
    else if (strcmp(tokens[0], "environ") == 0)
    {
        for (char **env = environ; *env; env++)
        {
            puts(*env);
        }
        return 1;
    }
    //
    else if (strcmp(tokens[0], "echo") == 0)
    {
        // if no args was entered then just print a new line
        if (numOfArgs == 1)
        {
            puts("");
        }
        else
        {
            for (int i = 1; i < numOfArgs; i++)
            {
                if (i == numOfArgs - 1)
                    printf("%s\n", tokens[i]);
                else
                    printf("%s ", tokens[i]);
                
            }
        }
        return 1;
    }
    else if (strcmp(tokens[0], "pause") == 0)
    {
        isPaused = 1;
        puts("Press 'Enter' to resume");
        
        char c;
        // loop until the enter key is pressed
        while ((c = getchar()) != '\n') {}
        
        isPaused = 0;
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
            // clear the paths array
            for (int i = 0; i < MAX_PATH_SIZE; i++)
            {
                paths[i] = NULL;
            }
            
            // copy new paths to array
            for (int i = 1; i < numOfArgs; i++)
            {
                paths[i-1] = strdup(tokens[i]);
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
    // list the contents of a directory
    else if (strcmp(tokens[0], "dir") == 0)
    {
        DIR* d;
        struct dirent *directory;
        // if not directory was pass in then assume they
        // want the contents of the current directory
        if (numOfArgs == 1)
            d = opendir(".");
        else
            d = opendir(tokens[1]);
        
        // couldn't open directory
        if (d == NULL)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }
        else
        {
            while ((directory = readdir(d)) != NULL)
            {
                printf("%s\n", directory->d_name);
            }
            closedir(d);
        }
        return 1;
    }
    return 0;
}

void execCommand(char* tokens[], int numOfArgs, int execInBackground)
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
        if (!execInBackground)
        {
            waitpid(pid, NULL, 0);
        }
        
    }
}

void pipeExec(char* tokens[], int numOfArgs)
{
    char* cmd1[10];
    char* cmd2[10];
    
    int sawPipe = 0;
    
    // find where the pip
    for (int i = 0, cmdIndex = 0; i < numOfArgs; i++)
    {
        if (*tokens[i] == '|')
        {
            cmdIndex = 0;
            sawPipe = 1;
        }
        else
        {
            if (sawPipe)
            {
                // add args to second command
                cmd2[cmdIndex++] = tokens[i];
            }
            else
            {
                // add args to first command
                cmd1[cmdIndex++] = tokens[i];
            }
        }
    }
    
    // file descriptor for reading and input
    int fd[2];
    int pid;
    
    // pipe created without error
    if (pipe(fd) == 0)
    {
        pid = fork();
        if (pid < 0)
        {
            // error
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }
        // child process
        if (pid == 0)
        {
            close(fd[0]);
            dup2(fd[1], 1);

            if (!builtInCommand(cmd1, numOfArgs))
            {
                if (execvp(cmd1[0], cmd1) < 0)
                {
                    // error
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    exit(1);
                }
            }
            
        }
        else
        {
            // parent process
            waitpid(pid, NULL, 0);
        }
        
        pid = fork();
        if (pid < 0)
        {
            // error
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }
        // child process
        if (pid == 0)
        {
            close(fd[1]);
            dup2(fd[0], 0);
            
            if (!builtInCommand(cmd2, numOfArgs))
            {
                if (execvp(cmd2[0], cmd2) < 0)
                {
                    // error
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    exit(1);
                }
            }
            
        }
        else
        {
            close(fd[0]);
            close(fd[1]);
            waitpid(pid, NULL, 0);
        }
    }
}

void backgroundExec(char* tokens[], int numOfArgs)
{
    // the current command to be executed
    char* cmd[10];
    
    int pidsList[10];
    int pidIndex = 0;
    
    int numOfCmdArgs = 0;
    int cmdIndex = 0;
    int restart = 0;
    
    int pid;
    
    for (int i = 0; i < numOfArgs; i++)
    {
        if (restart)
        {
            // clear the cmd array to setup for possible new cmd
            // clear array up to cmdIndex???
            *cmd[0] = '\0';
            cmdIndex = 0;
            restart = 0;
        }
        if (strcmp(tokens[i], "&") == 0)
        {
            // execue the cmd then look for more
            restart = 1;
            pid = fork();
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
                    if (accessStatus == 0) break;
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
                
                if (!builtInCommand(cmd, numOfCmdArgs))
                {
                    if (execvp(cmd[0], cmd) < 0)
                    {
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        exit(1);
                    }
                }
                
            }
            // parent process
            else
            {
                pidsList[pidIndex++] = pid;
            }
        }
        else
        {
            cmd[cmdIndex++] = tokens[i];
            numOfCmdArgs++;
        }
        
    }
    
    // wait for all background processes to complete before giving back control
    for (int i = 0; i < pidIndex; i++)
    {
        if (pidsList[i] != 0)
        {
            waitpid(pidsList[i], NULL, 0);
        }
    }
}

void inputRedirect(char* input[], int numOfArgs)
{
    char* cmd1[10];
    char* cmd2[10];
    
    char* cmdInput = malloc(20 * sizeof(char*));
    char* cmdOutput = malloc(20 * sizeof(char*));
    
    int foundFirstRedirection = 0;
    int cmdIndex = 0;
    
    enum input_redirection_type firstRedirectionType;
    enum input_redirection_type secondRedirectionType;
    
    // find the type of redirection to perform
    for (int i = 0; i < numOfArgs; i++)
    {
        // output redirection
        if (strcmp(input[i], ">") == 0 )
        {
            if (!foundFirstRedirection)
                firstRedirectionType = OUTPUT;
            else
                secondRedirectionType = OUTPUT;
            
            foundFirstRedirection = 1;
            // treat next index as output file
            strcpy(cmdOutput, input[++i]);
        }
        // output redirection appending to EOF
        else if (strcmp(input[i], ">>") == 0 )
        {
            if (!foundFirstRedirection)
                firstRedirectionType = OUTPUT_APPEND;
            else
                secondRedirectionType = OUTPUT_APPEND;
            
            foundFirstRedirection = 1;
            
            // treat next index as input file
            strcpy(cmdOutput, input[++i]);
        }
        // input redirection
        else if (strcmp(input[i], "<") == 0 )
        {
            if (!foundFirstRedirection)
                firstRedirectionType = INPUT;
            else
                secondRedirectionType = INPUT;
            
            foundFirstRedirection = 1;
            // treat next index as input file
            strcpy(cmdInput, input[++i]);
        }
        else
        {
            if (foundFirstRedirection)
                cmd2[cmdIndex++] = input[i];
            else
                cmd1[cmdIndex++] = input[i];
        }
    }
    
    // file descriptor
    int fd;
    int fdOut;
    int pid;
    
    if (firstRedirectionType == INPUT)
    {
        // open the file or create it if it doesn't exist
        fd = open(cmdInput, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        pid = fork();
        if (pid == 0)
        {
            // input contains both input and output redirection
            if (secondRedirectionType == OUTPUT_APPEND || secondRedirectionType == OUTPUT)
            {
                // cmd1 takes in the input file and the whole result is outputed to the output file
                if (secondRedirectionType == OUTPUT_APPEND)
                    fdOut = open(cmdOutput, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO);
                else
                    fdOut = open(cmdOutput, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
                // set the output file as the output file descriptor
                dup2(fdOut, 1);
                close(fdOut);
            }
            close(0);
            dup2(fd, 0);
            close(fd);

            if (!builtInCommand(cmd1, numOfArgs))
            {
                if (execvp(cmd1[0], cmd1) < 0)
                {
                    // error
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    exit(1);
                }
            }
            
        }
        else
        {
            // parent process
            waitpid(pid, NULL, 0);
        }
    }
    else if (firstRedirectionType == OUTPUT_APPEND || firstRedirectionType == OUTPUT)
    {
        
        printf("output: %s\n", cmdOutput);
        if (firstRedirectionType == OUTPUT_APPEND)
            fd = open(cmdOutput, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO);
        else
            fd = open(cmdOutput, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
        
        pid = fork();
        if (pid == 0)
        {
            close(1);
            dup2(fd, 1);
            close(fd);
            
            if (!builtInCommand(cmd1, numOfArgs))
            {
                if (execvp(cmd1[0], cmd1) < 0)
                {
                    // error
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    exit(1);
                }
            }
            
        }
        else
        {
            // parent process
            waitpid(pid, NULL, 0);
        }
        
    }
    // free resources
    free(cmdInput);
    free(cmdOutput);
    cmdInput = NULL;
    cmdOutput = NULL;
}
